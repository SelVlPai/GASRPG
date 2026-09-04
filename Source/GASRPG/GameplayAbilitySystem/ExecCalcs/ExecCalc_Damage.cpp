// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecCalc_Damage.h"
#include "GASRPG/GameplayAbilitySystem/AttributeSets/AttributeSetCharacter.h"
#include "GameplayTagContainer.h"
PRAGMA_DISABLE_OPTIMIZATION
struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(TargetDefense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(SourceDefense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(SkillDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Intelligence);

	FDamageStatics() 
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetCharacter, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetCharacter, Strength, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetCharacter, SkillDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetCharacter, Intelligence, Source, false);
		SourceDefenseDef = FGameplayEffectAttributeCaptureDefinition(UAttributeSetCharacter::GetDefenseAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAttributeSetCharacter, Health, Target, false);
		TargetDefenseDef = FGameplayEffectAttributeCaptureDefinition(UAttributeSetCharacter::GetDefenseAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const FDamageStatics& DamageStatics() 
{
	static FDamageStatics Statics;
	return Statics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().SourceDefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().TargetDefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().IntelligenceDef);
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().SkillDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecuteOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();


	float AttackDamage;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvalParams, AttackDamage);
	float TargetDefense;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().TargetDefenseDef, EvalParams, TargetDefense);
	float SourceDefense;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().SourceDefenseDef, EvalParams, SourceDefense);
	float Intelligence;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().IntelligenceDef, EvalParams, Intelligence);
	float Strength;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StrengthDef, EvalParams, Strength);
	float SkillDamage;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().SkillDamageDef, EvalParams, SkillDamage);
	
	
	const FGameplayTagContainer& AssetTags = Spec.DynamicAssetTags;

	float MainAttribute;
	FGameplayTag MainAttributeTag;

	if (AssetTags.HasTag(FGameplayTag::RequestGameplayTag("Data.Attributes.Intelligence"))) 
	{
		MainAttributeTag = FGameplayTag::RequestGameplayTag("Data.Attributes.Intelligence");
		MainAttribute = Intelligence;
	}
	else if(AssetTags.HasTag(FGameplayTag::RequestGameplayTag("Data.Attributes.Strength"))) 
	{
		MainAttributeTag = FGameplayTag::RequestGameplayTag("Data.Attributes.Strength");
		MainAttribute = Strength;
	}
	else if(AssetTags.HasTag(FGameplayTag::RequestGameplayTag("Data.Attributes.Defense"))) 
	{
		MainAttributeTag = FGameplayTag::RequestGameplayTag("Data.Attributes.Defense");
		MainAttribute = SourceDefense;
	}

	const float MultiplierMainAttribute = Spec.GetSetByCallerMagnitude(MainAttributeTag, false, 0);
	const float MultiplierSkillDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage.MultiplierSkillDamage"), false, 0);
	const float BaseSkillDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage.BaseSkillDamage"), false, 0);
	const float MultiplierEachLevel = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage.MultiplierEachLevel"), false, 0);
	const float AbilityLevel = Spec.GetLevel() - 1;

	float FullDamage;
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%f"), BaseSkillDamage));
	if (BaseSkillDamage == 0) 
	{

		const float FireDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage.Fire"), false, 0);
		FullDamage = FireDamage;
	}
	else 
	{ // 1.28
		float CalculateMultiplierMainAttribute = 1 + (MainAttribute * MultiplierMainAttribute / 100.f);
		// 1.24
		float CalculateMultiplierSkillDamage = 1 + (SkillDamage * MultiplierSkillDamage / 100.f);
		FullDamage = BaseSkillDamage * (1 + (AbilityLevel * MultiplierEachLevel) ) * CalculateMultiplierMainAttribute * CalculateMultiplierSkillDamage;
		// 15.874
	}
	float DefenseMultiplier = 100.f / (100.f + TargetDefense);

	//float FinalDamage = 10.f * (AttackDamage * 1.2f) * (AbilityLevel * 5.f);
	float FinalDamage = FullDamage * DefenseMultiplier;

	if (EvalParams.TargetTags->HasTag(FGameplayTag::RequestGameplayTag("State.Blind"))) 
	{
		FinalDamage *= 1.5f;
	}
	if (EvalParams.TargetTags && EvalParams.TargetTags->HasTag(FGameplayTag::RequestGameplayTag("State.Marked")))
	{
		FinalDamage *= 1.4f; // +40% шкоди по позначеній цілі
	}
	if (ShatterActivate(TargetASC, SourceASC)) 
	{	
		
			FinalDamage *= 2.0f;
			FGameplayEffectSpec* MutableSpec = const_cast<FGameplayEffectSpec*>(&Spec);
			// Додаємо динамічний тег прямо в Spec
			if (MutableSpec)
			{

				FGameplayTag ShatterTag = FGameplayTag::RequestGameplayTag(FName("Event.ShatterTriggered"));
				MutableSpec->AddDynamicAssetTag(ShatterTag);
			}
	}

	//const float BackstabMultiplier = Spec.GetSetByCallerMagnitude(
	//	FGameplayTag::RequestGameplayTag("Data.Backstab"),
	//	false, // не логувати помилку якщо тег не знайдено
	//	1.f    // дефолтне значення якщо тег відсутній
	//);
	//FinalDamage *= BackstabMultiplier;

	OutExecuteOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UAttributeSetCharacter::GetDamageAttribute(), EGameplayModOp::Additive,
		FinalDamage));

}

bool UExecCalc_Damage::ShatterActivate(UAbilitySystemComponent* TargetASC, UAbilitySystemComponent* SourceASC) const
{
	if (TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Frozen")))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("1")));
		FGameplayTagContainer Tags;
		Tags.AddTag(FGameplayTag::RequestGameplayTag("State.Frozen"));
		TArray<FActiveGameplayEffectHandle> ActiveEffects = TargetASC->GetActiveEffectsWithAllTags(Tags);
		if (!ActiveEffects.IsEmpty())
		{
			const FActiveGameplayEffect* Effect = TargetASC->GetActiveGameplayEffect(ActiveEffects[0]);
			if (Effect)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("2")));

				AActor* FrozenInstagator = Effect->Spec.GetContext().GetOriginalInstigator();

				AActor* CurrentAttacker = SourceASC->GetAvatarActor();
				if (FrozenInstagator && CurrentAttacker)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("3")));
					return CurrentAttacker != FrozenInstagator;

				}
			}
		}
	}
	return false;
}
PRAGMA_ENABLE_OPTIMIZATION
