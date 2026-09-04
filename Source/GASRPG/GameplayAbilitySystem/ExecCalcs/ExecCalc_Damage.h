// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecuteOutput) const override;
private:
	// Якщо на таргеті Frozen і вдарив не той хто наложив то демедж х2
	bool ShatterActivate(UAbilitySystemComponent* TargetASC, UAbilitySystemComponent* SourceASC) const;
};
