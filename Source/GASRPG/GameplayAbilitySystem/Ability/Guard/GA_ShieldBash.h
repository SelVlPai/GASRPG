// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASRPG/GameplayAbilitySystem/Ability/GA_Base.h"
#include "GA_ShieldBash.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_ShieldBash : public UGA_Base
{
	GENERATED_BODY()
	
public:
	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> StunEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> VulnerableEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float StunDuration = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BashRange = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BashRadius = 200.f;
private:
	ACharacter* FindTarget();

};
