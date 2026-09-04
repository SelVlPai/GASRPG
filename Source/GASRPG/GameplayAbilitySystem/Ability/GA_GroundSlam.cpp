// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_GroundSlam.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

void UGA_GroundSlam::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Cue
	if (ActorInfo->IsNetAuthority()) 
	{
		const FVector SlamLocation = Character->GetActorLocation();
		TArray<FOverlapResult> Overlaps;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(Character);

        GetWorld()->OverlapMultiByChannel(
            Overlaps,
            SlamLocation,
            FQuat::Identity,
            ECC_Pawn,
            FCollisionShape::MakeSphere(SlamRadius),
            Params
        );

        DrawDebugSphere(GetWorld(), SlamLocation,
            SlamRadius, 16, FColor::Orange, false, 2.f);
        for (FOverlapResult Overlap : Overlaps) 
        {
            ACharacter* Target = Cast<ACharacter>(Overlap.GetActor());
            if (!Target) continue;
            UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
            if (!TargetASC) continue;
            ApplyDamageToTarget(Target);
            FGameplayEffectSpecHandle SpecHandleSlow = CreateEffectSpecHandle(SlowEffect);
            if (SpecHandleSlow.IsValid()) 
            {
                SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandleSlow.Data.Get(), TargetASC);
            }
            LaunchTarget(Target, SlamLocation);
        }
	}
    EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
}

void UGA_GroundSlam::LaunchTarget(ACharacter* Target, FVector SlamLocation)
{
    FVector LaunchDirection = (Target->GetActorLocation() - SlamLocation).GetSafeNormal();
    LaunchDirection.Z = 0.f;
    LaunchDirection.Normalize();

    const FVector LaunchVelocity =
        LaunchDirection * LaunchStrength +
        FVector(0.f, 0.f, LaunchZStrength);

    
    Target->LaunchCharacter(LaunchVelocity, true, true);
}
