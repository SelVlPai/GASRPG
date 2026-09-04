// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Fortify.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

void UGA_Fortify::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

    // cue
    if (ActorInfo->IsNetAuthority()) 
    {
        if (!SourceASC)
        {
            EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, true);
            return;
        }
        FGameplayEffectSpecHandle SpecHandleFotifield = CreateEffectSpecHandle(FotifieldEffect);
        if (SpecHandleFotifield.IsValid())
        {
            TArray<ACharacter*> FindedAllAllies = FindAllies();
            if (FindedAllAllies.IsEmpty())
            {
                EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, true);
                return;
            }
            for (ACharacter* Ally : FindedAllAllies)
            {
                UAbilitySystemComponent* AllyASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Ally);
                if (!AllyASC) continue;
                
                SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandleFotifield.Data.Get(), AllyASC);
                
            }
        }
    }
    EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
   
}

TArray<ACharacter*> UGA_Fortify::FindAllies() const
{
    TArray<ACharacter*> Allies;
    if (!Character) return Allies;

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;

    GetWorld()->OverlapMultiByChannel(
        Overlaps,
        Character->GetActorLocation(),
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(AuraRadius),
        Params
    );

    DrawDebugSphere(GetWorld(),
        Character->GetActorLocation(),
        AuraRadius, 16, FColor::Blue, false, 3.f);

    APlayerState* OwnerPS = Character->GetPlayerState();

    for (FOverlapResult& Overlap : Overlaps)
    {
        ACharacter* OverlapChar = Cast<ACharacter>(Overlap.GetActor());
        if (!OverlapChar) continue;

        // Перевіряємо що це союзник а не ворог
        // Поки просто перевіряємо що це інший гравець з ASC
        // В майбутньому тут буде перевірка команди
        UAbilitySystemComponent* AllyASC =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
                OverlapChar);
        if (!AllyASC) continue;

        Allies.Add(OverlapChar);
    }

    return Allies;
}
