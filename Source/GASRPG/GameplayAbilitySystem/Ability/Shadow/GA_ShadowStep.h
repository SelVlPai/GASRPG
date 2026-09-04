// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASRPG/GameplayAbilitySystem/Ability/GA_Base.h"
#include "GA_ShadowStep.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class GASRPG_API UGA_ShadowStep : public UGA_Base
{
	GENERATED_BODY()
	
public:
	UGA_ShadowStep();

	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,bool ReplicatedAbility, bool WasCanclled) override;
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Animation")
	TObjectPtr<UAnimMontage> VanishMontage;
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Animation")
	TObjectPtr<UAnimMontage> AppearMontage;
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Combat")
	float BehindEnemyOffset;
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Combat")
	float MaxTeleportRange;

private:

	AActor* FindTarget(const FGameplayAbilityActorInfo* ActorInfo) const;
	void TeleportBehindTarget() const;
	/*void ApplyDamageToTarget(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const;*/

	TObjectPtr<AActor> Target;
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
