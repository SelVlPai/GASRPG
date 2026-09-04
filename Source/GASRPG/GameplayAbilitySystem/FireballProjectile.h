// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "FireballProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class GASRPG_API AFireballProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireballProjectile();

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayEffectSpecHandle DamageSpecHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayEffectSpecHandle BurnSpecHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereCollision;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(BlueprintReadOnly, Category = "Life")
	float LifeTime = 3.f;
};
