// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCulc_DoTDamage.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UExecCulc_DoTDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCulc_DoTDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecuteOutput) const override;
};
