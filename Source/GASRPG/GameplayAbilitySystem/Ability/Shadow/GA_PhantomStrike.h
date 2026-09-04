// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GASRPG/GameplayAbilitySystem/Ability/GA_Base.h"
#include "GA_PhantomStrike.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_PhantomStrike : public UGA_Base
{
	GENERATED_BODY()
	
public:
	UGA_PhantomStrike();
	virtual void PostInitProperties() override;
	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool ReplicatedAbility, bool WasCanclled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	float LongShoot = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	float ComboWindow = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	float HitMicroPause = 0.6;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> BleedEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float BleedDamageTick = 8.f;

private:

	void ExecuteHit(int32 HitIndex);
	ACharacter* FindTarget() const;
	void ApplyHitEffects(float DamageHit, bool ApplyBleed);
	UFUNCTION()
	void OnComboWindowExpired();
	UFUNCTION()
	void OnComboInputReceived(FGameplayEventData Payload);
	UFUNCTION()
	void OnMicroPauseFinished();
	UFUNCTION()
	void ApplyFinalCooldown();

	int32 CurrentHitIndex = 0.f;

	ACharacter* CachedTarget;

	float CD_1Hit;
	float CD_2Hit;
	float CD_3Hit;

	UPROPERTY(EditDefaultsOnly)
	float Multiplier_1Hit = 0.3f;
	UPROPERTY(EditDefaultsOnly)
	float Multiplier_2Hit = 0.5;
	float FinallMultiplier;

	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitGameplayEvent> EventTask;

	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitDelay> DelayTask;
	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitDelay> MicroPauseTask;

};
