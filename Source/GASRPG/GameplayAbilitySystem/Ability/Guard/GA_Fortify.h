// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASRPG/GameplayAbilitySystem/Ability/GA_Base.h"
#include "GA_Fortify.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_Fortify : public UGA_Base
{
	GENERATED_BODY()
public:

	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> FotifieldEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AuraRadius = 500.f;

private:

	TArray<ACharacter*> FindAllies() const;
};
