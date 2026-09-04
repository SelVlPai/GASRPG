// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Base.generated.h"

class UPDA_DefaultEffects;
/**
 * 
 */
UCLASS()
class GASRPG_API UGA_Base : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Base();

public:

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	bool ManualCooldown = false;
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float StaminaCost = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTag CooldownTag;

	UPROPERTY(EditDefaultsOnly, Category = "Multipliers")
	FGameplayTag NameMainAttribute;
	UPROPERTY(EditDefaultsOnly, Category = "Multipliers")
	float MultiplierMainAttribute = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Multipliers")
	float MultiplierEachLevel = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Multipliers")
	float MultiplierSkillDamage = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 10.f;

	UPROPERTY()
	TObjectPtr<ACharacter> Character;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FString AbilityTagName;


	void ApplyDamageToTarget(AActor* Target, float DamageMultiplier = 1.f) const;
	void ApplyDoTToTarget(AActor* Target, TSubclassOf<UGameplayEffect> DoTEffect, float DamagePerTick) const;
	void ApplyAllMagnitudeForDamage(FGameplayEffectSpecHandle& SpecHandle, float DamageMultiplier = 1.f) const;
	FGameplayEffectSpecHandle CreateEffectSpecHandle(TSubclassOf<UGameplayEffect> Effect) const;

	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) {};

};
