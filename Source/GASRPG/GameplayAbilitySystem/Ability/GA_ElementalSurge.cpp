// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ElementalSurge.h"
#include "AbilitySystemComponent.h"

void UGA_ElementalSurge::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (ActorInfo->IsNetAuthority() && SourceASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Surge")))
	{
		FGameplayTagContainer SurgeTags;
		SurgeTags.AddTag(FGameplayTag::RequestGameplayTag("State.Surge"));
		SourceASC->RemoveActiveEffectsWithGrantedTags(SurgeTags);
	}
	FGameplayEffectSpecHandle Spec = CreateEffectSpecHandle(SurgeEffect);
	if (Spec.IsValid())
	{
		SourceASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
	// Cue
	EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
}
