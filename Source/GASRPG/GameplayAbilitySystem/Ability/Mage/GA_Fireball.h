// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASRPG/GameplayAbilitySystem/Ability/GA_Base.h"
#include "GA_Fireball.generated.h"

class AFireballProjectile;

UCLASS()
class GASRPG_API UGA_Fireball : public UGA_Base
{
	GENERATED_BODY()
public:

	virtual void OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Fireball")
	TSubclassOf<AFireballProjectile> FireballProjectile;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> BurnEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Fireball")
	float DamageBurnPerTick;
	UPROPERTY(EditDefaultsOnly, Category = "Fireball")
	FVector SpawnOffset = FVector(100.f, 0.f, 0.f);
	UPROPERTY(EditDefaultsOnly, Category = "Fireball")
	float DurationEffect = 2.f;

};
