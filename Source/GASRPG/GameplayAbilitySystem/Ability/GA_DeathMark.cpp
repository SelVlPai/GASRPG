// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_DeathMark.h"
#include <Camera/CameraComponent.h>
#include "GameFramework/Character.h"
#include <AbilitySystemBlueprintLibrary.h>
#include "AbilitySystemComponent.h"

void UGA_DeathMark::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    ACharacter* Target = FindTarget();
    if (!Target)
    {
        EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
        return;
    }

    if (ActorInfo->IsNetAuthority() && DeathMark) 
    {
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
        if (!TargetASC && !SourceASC) return;
        FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
        Context.AddSourceObject(Character);

        FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DeathMark, GetAbilityLevel(), Context);
        if (!SpecHandle.IsValid()) return;

        FActiveGameplayEffectHandle ActiveHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

        ActorInfo->AbilitySystemComponent->ExecuteGameplayCue(
            FGameplayTag::RequestGameplayTag("GameplayCue.DeathMark.Apply"),
            ActorInfo->AbilitySystemComponent->MakeEffectContext()
        );
    }
    EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);

}

ACharacter* UGA_DeathMark::FindTarget() const
{
    if (!Character) return nullptr;

    UCameraComponent* Camera =
        Character->FindComponentByClass<UCameraComponent>();
    const FVector TraceStart = Camera
        ? Camera->GetComponentLocation()
        : Character->GetActorLocation();
    const FVector TraceEnd = TraceStart +
        Character->GetControlRotation().Vector() * MarkRange;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Character);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, TraceStart, TraceEnd, ECC_Pawn, Params);

    DrawDebugLine(GetWorld(), TraceStart, TraceEnd,
        bHit ? FColor::Red : FColor::White, false, 2.f);

    if (!bHit) return nullptr;

    if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
        Hit.GetActor()))
        return nullptr;

    return Cast<ACharacter>(Hit.GetActor());
}
