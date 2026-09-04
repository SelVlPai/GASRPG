// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ChainLightning.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

void UGA_ChainLightning::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// cue
	if (!ActorInfo->IsNetAuthority())
	{
		EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	SurgeActive = SourceASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Surge"));
	//TargetsChain.Empty();
	ACharacter* FirstTarget = FindFirstTarget();
	if (!FirstTarget)
	{
		EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	TargetsChain.Add(Character);
	// Cue
	/*ActorInfo->AbilitySystemComponent->ExecuteGameplayCue(
		FGameplayTag::RequestGameplayTag("GameplayCue.ChainLightning.Strike"),
		ActorInfo->AbilitySystemComponent->MakeEffectContext()*/
	ApplychainEffect(FirstTarget, 1.f);
	TargetsChain.Add(FirstTarget);
	ACharacter* CurrentTarget = FirstTarget;
	for (uint8 i = 1; i <= MaxChain; i++) 
	{
		ACharacter* NextTarget = FindChainTarget(CurrentTarget);
		if (!NextTarget) break;
		float MultiplierDamage = 1.f - 0.3f * i;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%f"), MultiplierDamage));
		ApplychainEffect(NextTarget, MultiplierDamage);
		CurrentTarget = NextTarget;
		TargetsChain.Add(CurrentTarget);
	}
	EndAbility(SpecHandle, ActorInfo, ActivationInfo, true, false);
}

ACharacter* UGA_ChainLightning::FindFirstTarget()
{
	UCameraComponent* Camera =
		Character->FindComponentByClass<UCameraComponent>();
	const FVector TraceStart = Camera
		? Camera->GetComponentLocation()
		: Character->GetActorLocation();
	const FVector TraceEnd = TraceStart +
		Character->GetControlRotation().Vector() * RangeFirstShoot;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit, TraceStart, TraceEnd, ECC_Pawn, Params);

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd,
		FColor::Yellow, false, 2.f);

	if (!bHit) return nullptr;

	if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		Hit.GetActor()))
		return nullptr;

	return Cast<ACharacter>(Hit.GetActor());
}

ACharacter* UGA_ChainLightning::FindChainTarget(ACharacter* TargetStartingPoint)
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;

	// Ігноруємо всіх вже вражених
	for (ACharacter* HitActor : TargetsChain)
	{
		Params.AddIgnoredActor(HitActor);
	}

	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		TargetStartingPoint->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(RadiusChain),
		Params
	);

	DrawDebugSphere(GetWorld(),
		TargetStartingPoint->GetActorLocation(),
		RadiusChain, 12, FColor::Yellow, false, 2.f);

	// Знаходимо найближчу невражену ціль з ASC
	ACharacter* ClosestTarget = nullptr;
	float ClosestDist = FLT_MAX;

	for (FOverlapResult& Overlap : Overlaps)
	{
		ACharacter* HitChar = Cast<ACharacter>(Overlap.GetActor());
		if (!HitChar) continue;

		if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			HitChar)) continue;

		const float Dist = FVector::Dist(
			TargetStartingPoint->GetActorLocation(),
			HitChar->GetActorLocation());

		if (Dist < ClosestDist)
		{
			ClosestDist = Dist;
			ClosestTarget = HitChar;
		}
	}

	return ClosestTarget;
}

void UGA_ChainLightning::ApplychainEffect(ACharacter* Target, float Multiplier)
{
	ApplyDamageToTarget(Target, Multiplier);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (TargetASC && SourceASC)
	{
		FGameplayEffectSpecHandle SpecHandleShocked = CreateEffectSpecHandle(ShockedEffect);
		if (SpecHandleShocked.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandleShocked.Data.Get(), TargetASC);
		}
		FGameplayEffectSpecHandle SpecHandleVulnerable = CreateEffectSpecHandle(VulnerableEffect);
		if (SurgeActive) 
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandleVulnerable.Data.Get(), TargetASC);
		}
	}
}
