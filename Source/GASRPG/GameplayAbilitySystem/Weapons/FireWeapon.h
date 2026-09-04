// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireWeapon.generated.h"

UCLASS()
class GASRPG_API AFireWeapon : public AActor
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshWeapon;

public:	
	// Sets default values for this actor's properties
	AFireWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
