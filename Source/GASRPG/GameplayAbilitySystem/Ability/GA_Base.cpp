// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Base.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GASRPG/GameplayAbilitySystem/PDA_DefaultEffects.h"
#include "GameFramework/Character.h"
#include "GASRPG/GameplayAbilitySystem/AttributeSets/AttributeSetCharacter.h"

UGA_Base::UGA_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	/*CostGameplayEffectClass = ;
	CooldownGameplayEffectClass = ;*/
}

const FGameplayTagContainer* UGA_Base::GetCooldownTags() const
{
	static FGameplayTagContainer StaticCooldownContainer;

	StaticCooldownContainer.Reset();

	if (CooldownTag.IsValid()) 
	{
		StaticCooldownContainer.AddTag(CooldownTag);
	}
	if (StaticCooldownContainer.IsEmpty())
	{
		return Super::GetCooldownTags();
	}
	return &StaticCooldownContainer;
}

void UGA_Base::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* Cooldown = GetCooldownGameplayEffect();
	if (!Cooldown) return;
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Cooldown->GetClass(), GetAbilityLevel());
	if (!SpecHandle.IsValid()) return;
	
	SpecHandle.Data->DynamicGrantedTags.Reset();

	if (CooldownTag.IsValid()) 
	{
		SpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
	}
	
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Cooldown.Duration"), CooldownDuration);

	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

bool UGA_Base::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	// 2. якщо StaminaCost <= 0, варт≥сть перев≥р€ти не потр≥бно
	if (StaminaCost <= 0.f)
	{
		return true;
	}

	// 3. ќтримуЇмо AttributeSet персонажа ≥ перев≥р€Їмо стам≥ну вручну
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		const UAttributeSetCharacter* AttributeSet = ActorInfo->AbilitySystemComponent->GetSet<UAttributeSetCharacter>();
		if (AttributeSet)
		{
			// ѕерев≥р€Їмо, чи поточна стам≥на б≥льша або дор≥внюЇ вартост≥
			return AttributeSet->GetStamina() >= StaminaCost;
		}
	}
	return false;
}

void UGA_Base::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* Cost = GetCostGameplayEffect();
	if (!Cost) return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Cost->GetClass(), GetAbilityLevel());
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Cost.Amount"), -StaminaCost);

	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

void UGA_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//FGameplayTagContainer AbilityTagsContainer = AbilityTags;

	// ѕриклад A: ѕерев≥рити, чи маЇ ц€ аб≥л≥т≥ конкретний тег
	//if (AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Fortify"))))
	//{
	//	// ÷е Fortify!
	//}

	// ѕриклад B: ¬ит€гнути перший тег €к р€док (наприклад, дл€ логуванн€)
	//AbilityTagName = AbilityTags.First().ToString();

	if (ManualCooldown) 
	{
		if (!CommitAbilityCost(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}
	else
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}
	Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	SourceASC = GetAbilitySystemComponentFromActorInfo();
	OnActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}



void UGA_Base::ApplyDamageToTarget(AActor* Target, float DamageMultiplier) const
{
	if (!Target || !DamageEffect) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC || !SourceASC) return;

	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.AddSourceObject(GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, GetAbilityLevel(), Context);
	if (!SpecHandle.IsValid()) return;
	if (NameMainAttribute.IsValid()) 
	{
		SpecHandle.Data->DynamicAssetTags.AddTag(NameMainAttribute);
	}
	ApplyAllMagnitudeForDamage(SpecHandle, DamageMultiplier);

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

void UGA_Base::ApplyDoTToTarget(AActor* Target, TSubclassOf<UGameplayEffect> DoTEffect, float DamagePerTick) const
{
	if (!Target || !DoTEffect) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC || !SourceASC) return;

	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.AddSourceObject(GetAvatarActorFromActorInfo());
	
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DoTEffect, GetAbilityLevel(), Context);
	if (!SpecHandle.IsValid()) return;
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.DoT.DamagePerTick"), DamagePerTick);

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

void UGA_Base::ApplyAllMagnitudeForDamage(FGameplayEffectSpecHandle& ModifiedSpecHandle, float DamageMultiplier) const
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Black, FString::Printf(TEXT("%f"), BaseDamage * DamageMultiplier));
	ModifiedSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage.BaseSkillDamage"), BaseDamage * DamageMultiplier);
	ModifiedSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage.MultiplierSkillDamage"), MultiplierSkillDamage);
	ModifiedSpecHandle.Data->SetSetByCallerMagnitude(NameMainAttribute, MultiplierMainAttribute);
	ModifiedSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage.MultiplierEachLevel"), MultiplierEachLevel);
}

FGameplayEffectSpecHandle UGA_Base::CreateEffectSpecHandle(TSubclassOf<UGameplayEffect> Effect) const
{
	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.AddSourceObject(Character);
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(Effect, GetAbilityLevel(), Context);
	return SpecHandle;
}
