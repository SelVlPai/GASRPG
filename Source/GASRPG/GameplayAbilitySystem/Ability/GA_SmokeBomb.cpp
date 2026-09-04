// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SmokeBomb.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"

UGA_SmokeBomb::UGA_SmokeBomb()
{
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.SmokeBomb"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.SmokeBomb"));
}

void UGA_SmokeBomb::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* EventData)
{
	FGameplayCueParameters CueParams;
	CueParams.Location = Character->GetActorLocation();
	CueParams.NormalizedMagnitude = 1.f;
	ActorInfo->AbilitySystemComponent->AddGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.SmokeBomb.Explode"), CueParams);
	if (ActorInfo->IsNetAuthority()) 
	{
		TArray<FOverlapResult> OverlapActors;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character);
		UAbilitySystemComponent* OwnerASC = ActorInfo->AbilitySystemComponent.Get();
		GetWorld()->OverlapMultiByChannel(OverlapActors, Character->GetActorLocation(), FQuat::Identity, ECC_Pawn,
			FCollisionShape::MakeSphere(BlastRadius), Params);

		DrawDebugSphere(GetWorld(), Character->GetActorLocation(), BlastRadius, 16, FColor::Green, false, 3.f);

		for (FOverlapResult& Result : OverlapActors) 
		{
			AActor* OverlapActor = Result.GetActor();
			if (!OverlapActor) continue;
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlapActor);
			if (!TargetASC) continue;

			if (TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Blind"))) continue;

			if (!BlindEffect) continue;
			if (OwnerASC != TargetASC)
			{
				FGameplayEffectContextHandle Context = OwnerASC->MakeEffectContext();
				Context.AddSourceObject(Character);

				FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(BlindEffect, GetAbilityLevel(), Context);

				if (SpecHandle.IsValid())
				{
					OwnerASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

				}
			}

		}
		EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
	}

}
