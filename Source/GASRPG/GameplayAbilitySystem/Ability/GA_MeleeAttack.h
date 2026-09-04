// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Base.h"
#include "GA_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_MeleeAttack : public UGA_Base
{
	GENERATED_BODY()

public:
	UGA_MeleeAttack();
	
	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MeleeRange;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MeleeRadius;
};
