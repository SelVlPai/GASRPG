// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_Base.h"
#include "GA_FrostNova.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_FrostNova : public UGA_Base
{
	GENERATED_BODY()
public:
	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	float Radius = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> FrozenEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float DurationEffect = 1.5f;
};
