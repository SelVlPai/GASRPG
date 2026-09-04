// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Base.h"
#include "GADash.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGADash : public UGA_Base
{
	GENERATED_BODY()
public:
	UGADash();
	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivatioInfo,const FGameplayEventData* TriggerEventData) override;

	/*virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivatioInfo,
		bool ReplicateEndAbility, bool WasCancelled) override;*/
	
	UFUNCTION()
	void OnDashFinished();
};
