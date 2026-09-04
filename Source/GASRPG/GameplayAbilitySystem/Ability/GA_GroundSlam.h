// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_Base.h"
#include "GA_GroundSlam.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_GroundSlam : public UGA_Base
{
	GENERATED_BODY()
public:
	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> SlowEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float SlamRadius = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float LaunchStrength = 800.f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float LaunchZStrength = 400.f;
private:
	void LaunchTarget(ACharacter* Target, FVector SlamLocation);
};
