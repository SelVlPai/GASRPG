// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASRPG/GameplayAbilitySystem/Ability/GA_Base.h"
#include "GA_SmokeBomb.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_SmokeBomb : public UGA_Base
{
	GENERATED_BODY()
	
public:

	UGA_SmokeBomb();

	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* EventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GA|Effect")
	TSubclassOf<UGameplayEffect> BlindEffect;

	UPROPERTY(EditDefaultsOnly, Category = "GA|Combat")
	float BlastRadius = 400.f;
};
