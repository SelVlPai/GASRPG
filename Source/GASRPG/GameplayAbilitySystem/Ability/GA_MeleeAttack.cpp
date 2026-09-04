// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MeleeAttack.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"

UGA_MeleeAttack::UGA_MeleeAttack()
{
}

void UGA_MeleeAttack::OnActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (ActorInfo->IsNetAuthority())
	{
		const FVector StatrtLocation = Character->GetActorLocation();
		const FVector ForwardVector = Character->GetActorForwardVector();
		const FVector HitLocation = StatrtLocation + ForwardVector * MeleeRange;

		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character);

		FColor Color = FColor::Red;
		if (GetWorld()->OverlapMultiByChannel(Overlaps, HitLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(MeleeRadius), Params))
		{
			for (FOverlapResult& Overlap : Overlaps)
			{
				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Overlap.GetActor());
				if (!TargetASC || !DamageEffect) continue;
				FGameplayEffectContextHandle Context = ActorInfo->AbilitySystemComponent->MakeEffectContext();
				Context.AddSourceObject(Character);

				FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(DamageEffect, GetAbilityLevel(), Context);

				if (SpecHandle.IsValid()) 
				{
					Color = FColor::Green;
					ActorInfo->AbilitySystemComponent.Get()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Findish"));
				}
			}
		}
		DrawDebugSphere(GetWorld(), HitLocation, MeleeRadius, 12, Color, false, 1);

		
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
