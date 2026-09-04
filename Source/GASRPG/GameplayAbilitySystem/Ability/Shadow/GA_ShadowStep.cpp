// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ShadowStep.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

UGA_ShadowStep::UGA_ShadowStep()
{
}

void UGA_ShadowStep::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	Target = FindTarget(ActorInfo);
	if (!Target) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Target") );
		EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, VanishMontage, 1, NAME_None, true);

	MontageTask->OnCompleted.AddDynamic(this, &UGA_ShadowStep::OnMontageCompleted);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_ShadowStep::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_ShadowStep::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_ShadowStep::OnMontageInterrupted);

	MontageTask->ReadyForActivation();
	ActorInfo->AbilitySystemComponent->AddGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.ShadowStep.Vanish"),
		ActorInfo->AbilitySystemComponent->MakeEffectContext());
}

void UGA_ShadowStep::OnMontageCompleted() 
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Montage"));
	const FGameplayAbilitySpecHandle SpecHandle = GetCurrentAbilitySpecHandle();
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

	if (!Character)
	{
		EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ActorInfo->IsNetAuthority()) 
	{
		TeleportBehindTarget();
		ApplyDamageToTarget(Target);
	}

	EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
};

void UGA_ShadowStep::OnMontageInterrupted() 
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
};

void UGA_ShadowStep::EndAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool ReplicatedAbility, bool WasCanclled)
{
	if (MontageTask && MontageTask->IsActive()) 
	{
		MontageTask->EndTask();
	}
	ActorInfo->AbilitySystemComponent->RemoveGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.ShadowStep.Vanish"));
	Super::EndAbility(SpecHandle, ActorInfo, ActivationInfo, ReplicatedAbility, WasCanclled);
}

AActor* UGA_ShadowStep::FindTarget(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!Character) return nullptr;

	UCameraComponent* Camera = Character->FindComponentByClass<UCameraComponent>();
	const FVector TraceStart = Camera ? Camera->GetComponentLocation() : Character->GetActorLocation();
	const FVector TraceEnd = TraceStart + Character->GetControlRotation().Vector() * MaxTeleportRange;

	FHitResult Result;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	const bool Hit = GetWorld()->LineTraceSingleByChannel(Result, TraceStart, TraceEnd, ECC_Pawn, Params);

	FColor Color = FColor::Red;
	if (Hit) Color = FColor::Green;
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 3);
	if (!Hit) return nullptr;

	if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Result.GetActor())) 
	{
		return nullptr;
	}
	return Result.GetActor();
}

void UGA_ShadowStep::TeleportBehindTarget() const
{
	const FVector TargetLcoation = Target->GetActorLocation();
	const FVector TargetForward = Target->GetActorForwardVector();
	const FVector TeleportLocation = TargetLcoation - TargetForward * BehindEnemyOffset;

	const FRotator LookAtTarget = (TargetLcoation - TeleportLocation).Rotation();
	Character->TeleportTo(TeleportLocation, LookAtTarget);
}

//void UGA_ShadowStep::ApplyDamageToTarget(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
//	const FGameplayAbilityActivationInfo ActivationInfo) const
//{
//	if (!DamageEffect) return;
//
//	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
//	if (!TargetASC) return;
//
//	FGameplayEffectContextHandle Context = ActorInfo->AbilitySystemComponent->MakeEffectContext();
//	Context.AddSourceObject(ActorInfo->AvatarActor.Get());
//
//	FGameplayEffectSpecHandle EffectSpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(DamageEffect, GetAbilityLevel(), Context);
//
//	if (EffectSpecHandle.IsValid()) 
//	{
//		ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), TargetASC);
//	}
//	else GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Effect"));
//}
