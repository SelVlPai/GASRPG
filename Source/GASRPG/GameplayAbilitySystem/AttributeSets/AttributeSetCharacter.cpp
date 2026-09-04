// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetCharacter.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Engine/OverlapResult.h"
#include <GameplayEffectComponents/TargetTagsGameplayEffectComponent.h>
#include <GASRPG/GameplayAbilitySystem/Character/MainCharacter.h>
#include "GASRPG/GameplayAbilitySystem/Framework/PlayerStateMiniGame.h"
#include "GASRPG/GameplayAbilitySystem/Framework/MainGameMode.h"

UAttributeSetCharacter::UAttributeSetCharacter()
{
	InitHealth(1);
	InitMaxHealth(1);
	InitStamina(1);
	InitMaxStamina(1);
	InitAttackPower(1);
	InitDefense(1);
	InitCritChance(1);
	InitSkillDamage(1);
	InitIntelligence(1);
	InitMoveSpeed(1);
	InitStrength(1);
	InitDamage(0.f);
}

void UAttributeSetCharacter::Dead(const FGameplayEffectModCallbackData& Data)
{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Activate1")));
		AActor* TargetActor = Data.Target.GetAvatarActor();
		if (!TargetActor) return;
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		FGameplayEffectContextHandle Contex = Data.EffectSpec.GetContext();
		if (TargetASC && TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Marked")))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Activate2")));
			UAbilitySystemComponent* SourceASC = Contex.GetInstigatorAbilitySystemComponent();
			if (!SourceASC) return;
			FGameplayTagContainer CooldownTags;
			CooldownTags.AddTag(FGameplayTag::RequestGameplayTag("Cooldown.DeathMark"));
			//SourceASC->RemoveActiveEffectsWithTags(CooldownTags);
			SourceASC->RemoveActiveEffectsWithGrantedTags(CooldownTags);
		}
		TargetASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Dead"));
		ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
		ACharacter* Killer = Cast<ACharacter>(Contex.GetEffectCauser());
		AController* TargetController = TargetCharacter->GetController();
		AController* KillerController = Killer->GetController();
		if (TargetController && KillerController) 
		{
			Cast<APlayerStateMiniGame>(TargetCharacter->GetPlayerState())->AddScoreInTable(Killer->GetController(), TargetCharacter->GetController());
		}
		else 
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Not")));
		}
		GetWorld()->GetAuthGameMode<AMainGameMode>()->BP_PlayerDead(TargetController, KillerController);
}


void UAttributeSetCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, SkillDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, CritChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetCharacter, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UAttributeSetCharacter::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute()) 
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}
	if (Attribute == GetStaminaAttribute()) 
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxStamina());
	}
}

void UAttributeSetCharacter::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute()) 
	{
		const float DamageCount = GetDamage();
		SetDamage(0);

		const float NewHealth = FMath::Clamp(GetHealth() - DamageCount, 0, GetMaxHealth());
		SetHealth(NewHealth);
		// Event from ExecCalc_Damage
		if (Data.EffectSpec.GetDynamicAssetTags().HasTag(FGameplayTag::RequestGameplayTag("Event.ShatterTriggered")))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Shatter Explosive Triggered in AttributeSet!"));
			ExecuteShatterExplosion(Data, DamageCount);
			// 1. Знімаємо State.Frozen з цілі
			// 2. Програємо GameplayCue
			// 3. Запускаємо AOE
		}
		if (DamageCount > 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT(" Activate ")));
			/*if (SourceASC && SourceASC->HasMatchingGameplayTag(
				FGameplayTag::RequestGameplayTag("State.Crit")))
			{
				CueParams.AggregatedSourceTags.AddTag(
					FGameplayTag::RequestGameplayTag("State.Crit"));
			}*/
			FGameplayEventData EventData;
			EventData.Target = Data.Target.GetAvatarActor();
			EventData.EventMagnitude = DamageCount;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Data.EffectSpec.GetContext().GetEffectCauser(),
				FGameplayTag::RequestGameplayTag("Event.DamageIndicator"), EventData);
		}

		if (NewHealth <= 0)
		{
			Dead(Data);
		}
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
}

void UAttributeSetCharacter::ExecuteShatterExplosion(const FGameplayEffectModCallbackData& Data, float TotalDamageDealt)
{
	AActor* Target = Data.Target.GetAvatarActor();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	UAbilitySystemComponent* SourceASC = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();

	FGameplayTag Tag = FGameplayTag::RequestGameplayTag("State.Frozen");
	TargetASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(Tag));
	// Cue
	UWorld* World = Target->GetWorld();
	if (!World) return;

	const float OriginalDamage = TotalDamageDealt / 2.0f;
	const float AOERadius = 300.0f;

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(AOERadius);

	FCollisionQueryParams QueryParams;
	/*QueryParams.AddIgnoredActor(Target);*/
	if (SourceASC->GetAvatarActor())
	{
		QueryParams.AddIgnoredActor(SourceASC->GetAvatarActor());
	}

	World->OverlapMultiByChannel(
		Overlaps,
		Target->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn, 
		Sphere,
		QueryParams
	);

	UGameplayEffect* DynamicDamageGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName("DynamicAOEDamage"));
	DynamicDamageGE->DurationPolicy = EGameplayEffectDurationType::Instant;

	// 2. Додаємо модифікатор для атрибута шкоди
	FGameplayModifierInfo ModInfo;
	ModInfo.Attribute = UAttributeSetCharacter::GetDamageAttribute(); // Твій атрибут шкоди
	ModInfo.ModifierOp = EGameplayModOp::Override; // або Add
	ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(OriginalDamage));;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%f - %f"), OriginalDamage, TotalDamageDealt));

	DynamicDamageGE->Modifiers.Add(ModInfo);

	////////////////////

	UGameplayEffect* DynamicSlowGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName("DynamicSlow"));
	DynamicSlowGE->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DynamicSlowGE->DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(2.0f)); // 2 секунди

	// Якщо потрібно зрізати швидкість (наприклад, на 30% / коефіцієнт 0.7):
	FGameplayModifierInfo SpeedMod;
	SpeedMod.Attribute = UAttributeSetCharacter::GetMoveSpeedAttribute();
	SpeedMod.ModifierOp = EGameplayModOp::MultiplyAdditive; // або Multiply
	SpeedMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(0.7f));
	DynamicSlowGE->Modifiers.Add(SpeedMod);

	// Додаємо тег State.Debuff.Slow (за потреби)
	UTargetTagsGameplayEffectComponent& TargetTagsComp = DynamicSlowGE->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	FInheritedTagContainer TagContainer;
	TagContainer.Added.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Slow")));
	TargetTagsComp.SetAndApplyTargetTagChanges(TagContainer);

	// 3. Застосовуємо до VictimASC
	/*FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	SourceASC->ApplyGameplayEffectToTarget(DynamicDamageGE, VictimASC, 1.0f, Context);*/

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Victim = Overlap.GetActor();
		if (!Victim) continue;
		UAbilitySystemComponent* VictimASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Victim);
		if (!VictimASC) continue;
		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		SourceASC->ApplyGameplayEffectToTarget(DynamicDamageGE, VictimASC, 1.0f, EffectContext);
		SourceASC->ApplyGameplayEffectToTarget(DynamicSlowGE, VictimASC, 1.0f, EffectContext);
		/*FGameplayEffectSpecHandle DamageSpecSlow = SourceASC->MakeOutgoingSpec(SlowEffect, 1.0f, EffectContext);
		if (DamageSpecSlow.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecSlow.Data.Get(), VictimASC);
		}
		/*FGameplayEffectSpecHandle DamageSpecDamage = SourceASC->MakeOutgoingSpec(DynamicDamageGE->StaticClass(), 1.0f, DamageContext);

		if (DamageSpecDamage.IsValid())
		{

			SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpecDamage.Data.Get(), VictimASC);
		}
		

		*/
	}
}

void UAttributeSetCharacter::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, Health, OldHealth);
	

}

void UAttributeSetCharacter::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, MaxHealth, OldMaxHealth);
	
}

void UAttributeSetCharacter::OnRep_Stamina(const FGameplayAttributeData& OldStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, Stamina, OldStamina);

}

void UAttributeSetCharacter::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, MaxStamina, OldMaxStamina);

}

void UAttributeSetCharacter::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, AttackPower, OldAttackPower)
}

void UAttributeSetCharacter::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, Strength, OldStrength)

}

void UAttributeSetCharacter::OnRep_Defense(const FGameplayAttributeData& OldDefense) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, Defense, OldDefense)
}

void UAttributeSetCharacter::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, Intelligence, OldIntelligence)
}

void UAttributeSetCharacter::OnRep_CritChance(const FGameplayAttributeData& OldCritChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, CritChance, OldCritChance)
}

void UAttributeSetCharacter::OnRep_SkillDamage(const FGameplayAttributeData& OldSkillDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, SkillDamage, OldSkillDamage)
}

void UAttributeSetCharacter::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetCharacter, MoveSpeed, OldMoveSpeed)
}
