// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASRPG/GameplayAbilitySystem/Ability/GA_Base.h"
#include "GA_DeathMark.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_DeathMark : public UGA_Base
{
	GENERATED_BODY()
	
public:

	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
private:
	ACharacter* FindTarget() const;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> DeathMark;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MarkRange = 1500.f;
};
