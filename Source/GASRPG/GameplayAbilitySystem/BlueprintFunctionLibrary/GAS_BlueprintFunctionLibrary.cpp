// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS_BlueprintFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

FGameplayTag UGAS_BlueprintFunctionLibrary::GetCooldownTagFromSpecHandle(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle SpecHandle)
{
	if (!ASC || !SpecHandle.IsValid()) return FGameplayTag();

	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(SpecHandle);
	if (!AbilitySpec) return FGameplayTag();

	UGameplayAbility* Ability = AbilitySpec->GetPrimaryInstance();
	if (!Ability) return FGameplayTag();

	const FGameplayTagContainer* CooldownTags = Ability->GetCooldownTags();

	if (CooldownTags && CooldownTags->Num() > 0) 
	{
		return CooldownTags->GetByIndex(0);
	}

	return FGameplayTag();
}
