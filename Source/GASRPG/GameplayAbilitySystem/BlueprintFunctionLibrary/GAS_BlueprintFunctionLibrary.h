// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GAS_BlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API UGAS_BlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "GAS|UI")
	static FGameplayTag GetCooldownTagFromSpecHandle(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle SpecHandle);
};
