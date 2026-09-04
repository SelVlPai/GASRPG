// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_FrostNova.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

void UGA_FrostNova::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Cue
	const bool SurgeActive = SourceASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Surge"));
	float Duraction = DurationEffect;
	if (SurgeActive)
	{
		Duraction *= 2.f;
	}
	if (ActorInfo->IsNetAuthority()) 
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character);

		GetWorld()->OverlapMultiByChannel(Overlaps, Character->GetActorLocation(), FQuat::Identity, ECC_Pawn,
			FCollisionShape::MakeSphere(Radius), Params);
		DrawDebugSphere(GetWorld(),
			Character->GetActorLocation(),
			Radius, 16, FColor::Cyan, false, 2.f);

		for (FOverlapResult Overlap : Overlaps) 
		{
			AActor* Target = Overlap.GetActor();
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
			if (!TargetASC) continue;
			if (TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Frozen"))) continue;
			ApplyDamageToTarget(Target);
			if (FrozenEffect) 
			{
				FGameplayEffectSpecHandle SpecHandle = CreateEffectSpecHandle(FrozenEffect);
				if (SpecHandle.IsValid()) 
				{
					SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Duration.Effect"), Duraction);
					SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				}
			}
		}
		if (SurgeActive)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Remove")));
			FGameplayTagContainer SurgeTags;
			SurgeTags.AddTag(FGameplayTag::RequestGameplayTag("State.Surge"));
			SourceASC->RemoveActiveEffectsWithGrantedTags(SurgeTags);
		}
	}
	
	EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
	
}

