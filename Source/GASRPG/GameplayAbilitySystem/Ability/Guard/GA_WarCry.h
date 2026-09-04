// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASRPG/GameplayAbilitySystem/Ability/GA_Base.h"
#include "GA_WarCry.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGA_WarCry : public UGA_Base
{
	GENERATED_BODY()
public:

	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> WarCryEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float Radius = 800.f;
private:
	TArray<ACharacter*> FindAliies()const;
};
