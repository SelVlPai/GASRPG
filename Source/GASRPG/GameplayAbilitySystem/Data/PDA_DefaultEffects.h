// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "PDA_DefaultEffects.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UPDA_DefaultEffects : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CooldownEffect;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CostEffect;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;
};
