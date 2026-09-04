// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_WarCry.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

void UGA_WarCry::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//Cue
	if(ActorInfo->IsNetAuthority())
	{
	
		FGameplayEffectSpecHandle SpecHandleWarCry = CreateEffectSpecHandle(WarCryEffect);
		if (SpecHandleWarCry.IsValid()) 
		{
			TArray<ACharacter*> FindedAllAllies = FindAliies();
			if (FindedAllAllies.IsEmpty())
			{
				EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, true);
				return;
			}
			for (ACharacter* Ally : FindedAllAllies) 
			{
				UAbilitySystemComponent* AllyASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Ally);
				if (!AllyASC || !SourceASC) continue;
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandleWarCry.Data.Get(), AllyASC);
			}
		}
	}
	EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);

}

TArray<ACharacter*> UGA_WarCry::FindAliies()const
{
    TArray<ACharacter*> Allies;
    if (!Character) return Allies;

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
   // Params.AddIgnoredActor(Character);

    GetWorld()->OverlapMultiByChannel(
        Overlaps,
        Character->GetActorLocation(),
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(Radius),
        Params
    );

    DrawDebugSphere(GetWorld(),
        Character->GetActorLocation(),
        Radius, 16, FColor::Yellow, false, 3.f);

    for (FOverlapResult& Overlap : Overlaps)
    {
        ACharacter* OverlapChar = Cast<ACharacter>(Overlap.GetActor());
        if (!OverlapChar) continue;

        UAbilitySystemComponent* AllyASC =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
                OverlapChar);
        if (!AllyASC) continue;

        Allies.Add(OverlapChar);
    }

    return Allies;
	
}
