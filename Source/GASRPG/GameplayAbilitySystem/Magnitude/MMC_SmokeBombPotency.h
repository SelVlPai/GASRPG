// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_SmokeBombPotency.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UMMC_SmokeBombPotency : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_SmokeBombPotency();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& EffectSpec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition AttackPowerDef;
};
