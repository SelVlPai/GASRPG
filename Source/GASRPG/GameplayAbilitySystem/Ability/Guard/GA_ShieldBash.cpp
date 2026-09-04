// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ShieldBash.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

void UGA_ShieldBash::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ACharacter* Target = FindTarget();
	if (!Target) 
	{
		EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	//Cue
	if (ActorInfo->IsNetAuthority()) 
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
		if (!TargetASC || !SourceASC) return;
		ApplyDamageToTarget(Target);
		FGameplayEffectSpecHandle SpecHandleStun = CreateEffectSpecHandle(StunEffect);
		if (SpecHandleStun.IsValid()) 
		{		
			SpecHandleStun.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Duration.Effect"), StunDuration);
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandleStun.Data.Get(), TargetASC);
		}
		FGameplayEffectSpecHandle SpecHandleVulnerable = CreateEffectSpecHandle(VulnerableEffect);
		if (SpecHandleVulnerable.IsValid())
		{		
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandleVulnerable.Data.Get(), TargetASC);
		}
	}
    EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
}

ACharacter* UGA_ShieldBash::FindTarget()
{
    if (!Character) return nullptr;

    const FVector StartLocation = Character->GetActorLocation();
    const FVector ForwardLocation = StartLocation +
        Character->GetActorForwardVector() * BashRange;

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Character);

    GetWorld()->OverlapMultiByChannel(
        Overlaps,
        ForwardLocation,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(BashRadius),
        Params
    );

    DrawDebugSphere(GetWorld(), ForwardLocation,
        BashRadius, 12, FColor::Blue, false, 2.f);

    ACharacter* ClosestTarget = nullptr;
    float ClosestDist = FLT_MAX;

    for (FOverlapResult& Overlap : Overlaps)
    {
        ACharacter* HitChar = Cast<ACharacter>(Overlap.GetActor());
        if (!HitChar) continue;

        if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
            HitChar)) continue;

        const float Dist = FVector::Dist(
            Character->GetActorLocation(),
            HitChar->GetActorLocation());

        if (Dist < ClosestDist)
        {
            ClosestDist = Dist;
            ClosestTarget = HitChar;
        }
    }

    return ClosestTarget;
}
