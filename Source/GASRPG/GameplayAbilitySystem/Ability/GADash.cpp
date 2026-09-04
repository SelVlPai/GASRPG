// Fill out your copyright notice in the Description page of Project Settings.


#include "GADash.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include <GameFramework/RootMotionSource.h>

UGADash::UGADash()
{
}

void UGADash::OnActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivatioInfo, const FGameplayEventData* TriggerEventData)
{
	FVector DirectionAbility = Character->GetLastMovementInputVector();
	if (DirectionAbility.IsNearlyZero())
		DirectionAbility = Character->GetActorForwardVector();
	UAbilityTask_ApplyRootMotionConstantForce* DashTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, FName("DashTask"),
		DirectionAbility, 2000.f, 0.3f, false, nullptr, ERootMotionFinishVelocityMode::ClampVelocity,
		FVector::ZeroVector, Character->GetMovementComponent()->GetMaxSpeed(), false);
	if (DashTask)
	{
		DashTask->OnFinish.AddDynamic(this, &UGADash::OnDashFinished);
		DashTask->ReadyForActivation();
	}
	else
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

//void UGADash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
//	const FGameplayAbilityActivationInfo ActivatioInfo, bool ReplicateEndAbility, bool WasCancelled)
//{
//}

void UGADash::OnDashFinished()
{
	if (CurrentActorInfo && CurrentSpecHandle.IsValid())
	{
		if (CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
