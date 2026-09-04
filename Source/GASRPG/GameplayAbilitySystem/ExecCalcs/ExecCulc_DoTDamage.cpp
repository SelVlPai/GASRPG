// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecCulc_DoTDamage.h"
#include "GASRPG/GameplayAbilitySystem/AttributeSets/AttributeSetCharacter.h"

UExecCulc_DoTDamage::UExecCulc_DoTDamage()
{
}

void UExecCulc_DoTDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecuteOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float DamagePerTick = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.DoT.DamagePerTick"), false, 0.f);

	if (EvalParams.TargetTags && EvalParams.TargetTags->HasTag(FGameplayTag::RequestGameplayTag("State.Vulnerable"))) 
	{
		DamagePerTick *= 1.2f;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("DamageTick : %f"), DamagePerTick));
	OutExecuteOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UAttributeSetCharacter::GetDamageAttribute(),
		EGameplayModOp::Additive, DamagePerTick));
}
