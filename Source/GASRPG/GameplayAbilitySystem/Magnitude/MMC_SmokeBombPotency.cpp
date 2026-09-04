// Fill out your copyright notice in the Description page of Project Settings.


#include "MMC_SmokeBombPotency.h"
#include "GASRPG/GameplayAbilitySystem/AttributeSets/AttributeSetCharacter.h"
UMMC_SmokeBombPotency::UMMC_SmokeBombPotency()
{
	AttackPowerDef.AttributeToCapture = UAttributeSetCharacter::GetAttackPowerAttribute();
	AttackPowerDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	AttackPowerDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(AttackPowerDef);
}

float UMMC_SmokeBombPotency::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& EffectSpec) const
{
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float AttackPower;
	GetCapturedAttributeMagnitude(AttackPowerDef, EffectSpec, EvalParams, AttackPower);

	const float AbilityLevel = EffectSpec.GetLevel();
	const float Potency = 2.f * (AttackPower * 0.04f) * (AbilityLevel * 0.5f);
	return Potency;
}
