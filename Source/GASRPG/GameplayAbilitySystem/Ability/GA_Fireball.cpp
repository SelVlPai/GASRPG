// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Fireball.h"
#include <Camera/CameraComponent.h>
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "GASRPG/GameplayAbilitySystem/FireballProjectile.h"

void UGA_Fireball::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	const bool SurgeActive = SourceASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Surge"));
	float Duraction = DurationEffect;
	float DamageBurn = DamageBurnPerTick;
	if (SurgeActive) 
	{
		Duraction *= 2.f;
		DamageBurn *= 2.f;
	}
	if (ActorInfo->IsNetAuthority()) 
	{
		UCameraComponent* Camera = Character->FindComponentByClass<UCameraComponent>();
		FVector SpawnLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * SpawnOffset.X + FVector(0.f, 0.f, SpawnOffset.Z);
		FRotator SpawnRotation = Camera->GetComponentRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Character;
		SpawnParams.Instigator = Character;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		
		AFireballProjectile* Fireball = GetWorld()->SpawnActor<AFireballProjectile>(FireballProjectile, SpawnLocation, SpawnRotation, SpawnParams);
		if (Fireball) 
		{	
			FGameplayEffectSpecHandle DamageSpecHandle = CreateEffectSpecHandle(DamageEffect);
			if (DamageSpecHandle.IsValid())
			{
				ApplyAllMagnitudeForDamage(DamageSpecHandle);
				Fireball->DamageSpecHandle = DamageSpecHandle;
			}
			FGameplayEffectSpecHandle BurnSpecHandle = CreateEffectSpecHandle(BurnEffect);
			if (BurnSpecHandle.IsValid())
			{
				BurnSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.DoT.DamagePerTick"), DamageBurn);
				BurnSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Duration.Effect"), Duraction);
				Fireball->BurnSpecHandle = BurnSpecHandle;
			}
		}
		if (SurgeActive)
		{
			FGameplayTagContainer SurgeTags;
			SurgeTags.AddTag(FGameplayTag::RequestGameplayTag("State.Surge"));
			SourceASC->RemoveActiveEffectsWithGrantedTags(SurgeTags);
		}
	}
	
	EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
}
