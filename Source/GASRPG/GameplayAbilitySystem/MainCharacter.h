// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "MainCharacter.generated.h"

class UAbilitySystemComponent;
class UAttributeSetCharacter;

UCLASS()
class GASRPG_API AMainCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	void GiveAllAbilities();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> AllAbilitites;

public:	

	void Die();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAttributeSetCharacter> AttributeSetCharacter;


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_InitAttribute();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_OnBuffAdded(const FGameplayTag Tag, float Duraction);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_ChangeAttributesInWidget();

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SendBuffToUI(FGameplayTag Tag, float Duration);
	void Server_OnGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	void OnGameplayTagChanged(const FGameplayTag Tag, int32 NewCount);
	void InitAbilitySystemComponent();
	
};
