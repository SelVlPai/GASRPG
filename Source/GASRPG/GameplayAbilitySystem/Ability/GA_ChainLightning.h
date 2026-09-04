// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_Base.h"
#include "GA_ChainLightning.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_ChainLightning : public UGA_Base
{
	GENERATED_BODY()
	
public:
	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> ShockedEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> VulnerableEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float RangeFirstShoot = 1500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float RadiusChain = 600.f;

private:
	bool SurgeActive;
	uint8 MaxChain = 2;
	TArray<ACharacter*> TargetsChain;
	ACharacter* FindFirstTarget();
	ACharacter* FindChainTarget(ACharacter* TargetStartingPoint);
	void ApplychainEffect(ACharacter* Target, float Multiplier);
};
