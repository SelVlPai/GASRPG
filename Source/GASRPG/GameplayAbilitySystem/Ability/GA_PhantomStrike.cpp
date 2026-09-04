// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_PhantomStrike.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"


UGA_PhantomStrike::UGA_PhantomStrike() 
{
	
}
void UGA_PhantomStrike::PostInitProperties()
{
	Super::PostInitProperties();
	// комбо скинулося на 1 ударі то кд менше на 60%, на 2 ударі на 30%, 3 фул
	CD_1Hit = CooldownDuration * (1.f - 60.f / 100.f);
	CD_2Hit = CooldownDuration * (1.f - 30.f / 100.f);
	CD_3Hit = CooldownDuration;

	CD_1Hit = FMath::RoundToFloat(CD_1Hit * 10.f) / 10.f;
	CD_2Hit = FMath::RoundToFloat(CD_2Hit * 10.f) / 10.f;


}

void UGA_PhantomStrike::OnActivateAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CurrentHitIndex = 1;
	
	ExecuteHit(1);
}

void UGA_PhantomStrike::EndAbility(const FGameplayAbilitySpecHandle SpecHandle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool ReplicatedAbility, bool WasCanclled)
{
	if (EventTask && EventTask->IsActive()) 
	{
		EventTask->EndTask();
		
	}

	if (DelayTask && DelayTask->IsActive())
	{
		DelayTask->EndTask();
		if (GetCurrentActorInfo()->IsNetAuthority()) 
		{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Black, FString::Printf(TEXT("Ser:Active")));

		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Black, FString::Printf(TEXT("Local:Active")));
		}
		if (CurrentHitIndex == 1)
			CooldownDuration = CD_1Hit;
		else if (CurrentHitIndex == 2)
			CooldownDuration = CD_2Hit;
		ApplyFinalCooldown();
	}
		SourceASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("Combo.PhantomStrike.1Hit"));
		SourceASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("Combo.PhantomStrike.2Hit"));

	Super::EndAbility(SpecHandle, ActorInfo, ActivationInfo,
		ReplicatedAbility, WasCanclled);
}

void UGA_PhantomStrike::ExecuteHit(int32 HitIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%i"), CurrentHitIndex));
	CachedTarget = FindTarget();
	bool ApplyBleed;

	if (CurrentHitIndex == 1)
	{
		SourceASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("Combo.PhantomStrike.1Hit"));
		FinallMultiplier = Multiplier_1Hit;
		ApplyBleed = false;
	}
	else if (CurrentHitIndex == 2)
	{
		SourceASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("Combo.PhantomStrike.2Hit"));
		FinallMultiplier = Multiplier_2Hit;
		ApplyBleed = false;
	}
	else if (CurrentHitIndex == 3)
	{
		FinallMultiplier = 1.f;
		ApplyBleed = true;
	}

	if (GetCurrentActorInfo()->IsNetAuthority() && CachedTarget)
	{
		ApplyHitEffects(BaseDamage, ApplyBleed);
	}
	if (CurrentHitIndex >= 3)
	{
		/*if (GetCurrentActorInfo()->IsNetAuthority())
		{*/
			CooldownDuration = CD_3Hit;
			ApplyFinalCooldown();
		//}
			EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfoRef(), true, false);
			return;
		
	}

	MicroPauseTask = UAbilityTask_WaitDelay::WaitDelay(this, HitMicroPause);
	MicroPauseTask->OnFinish.AddDynamic(this, &UGA_PhantomStrike::OnMicroPauseFinished);
	MicroPauseTask->ReadyForActivation();
	
}
ACharacter* UGA_PhantomStrike::FindTarget() const
{
	if (!Character) return nullptr;

	const FVector StartLocation = Character->GetActorLocation();
	const FVector ForwardLocation = StartLocation + Character->GetActorForwardVector() * LongShoot;

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	GetWorld()->OverlapMultiByChannel(Overlaps, ForwardLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(150.f), Params);
	DrawDebugSphere(GetWorld(), ForwardLocation,
		150.f, 12, FColor::Orange, false, 2.f);
	ACharacter* ClosestTarget = nullptr;
	float ClosestDist = LongShoot + 100.f;

	for (FOverlapResult& Overlap : Overlaps)
	{
		ACharacter* Hit = Cast<ACharacter>(Overlap.GetActor());
		if (!Hit) continue;
		if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit)) continue;

	/*	const float Dist = FVector::Dist(Character->GetActorLocation(), Hit->GetActorLocation());
			ClosestDist = Dist;*/
			ClosestTarget = Hit;
	}

	return ClosestTarget;
}

void UGA_PhantomStrike::ApplyHitEffects(float DamageHit, bool ApplyBleed)
{
	if (!CachedTarget) return;
	ApplyDamageToTarget(CachedTarget, FinallMultiplier);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("ApplyDamage")));
	if (ApplyBleed && BleedEffect)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("ApllyEffect")));
		ApplyDoTToTarget(CachedTarget, BleedEffect, BleedDamageTick);
	}
}

void UGA_PhantomStrike::OnComboWindowExpired()
{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("%i"), CurrentHitIndex));
	if (EventTask && EventTask->IsActive())
	{
		EventTask->EndTask();
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%i"), CurrentHitIndex));
	//if (GetCurrentActorInfo()->IsNetAuthority())
	//{
		/*if (CurrentHitIndex == 1)
			CooldownDuration = CD_1Hit;
		else if (CurrentHitIndex == 2)
			CooldownDuration = CD_2Hit;*/
		//ApplyFinalCooldown();
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("%f"), CooldownDuration));
	//}
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	
}

void UGA_PhantomStrike::OnComboInputReceived(FGameplayEventData Payload)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Pressed")));
	if (DelayTask && DelayTask->IsActive()) 
	{
		DelayTask->EndTask();
	}
	if (MicroPauseTask && MicroPauseTask->IsActive())
	{
		MicroPauseTask->EndTask();
	}
	if (EventTask && EventTask->IsActive())
	{
		EventTask->EndTask();
	}
	CurrentHitIndex++;
	ExecuteHit(CurrentHitIndex);
}

void UGA_PhantomStrike::OnMicroPauseFinished()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Micro")));
	
	DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ComboWindow);
	DelayTask->OnFinish.AddDynamic(this, &UGA_PhantomStrike::OnComboWindowExpired);
	DelayTask->ReadyForActivation();

	EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Event.Abilities.ComboClick"), nullptr, false, true);
	EventTask->EventReceived.AddDynamic(this, &UGA_PhantomStrike::OnComboInputReceived);
	EventTask->ReadyForActivation();
}

void UGA_PhantomStrike::ApplyFinalCooldown()
{
	if (GetCurrentActorInfo()->IsNetAuthority())
	{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Ser:Apply")));
	}
	//ApplyCooldown(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
	CommitAbilityCooldown(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	//ApplyCooldown(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
}
