// Fill out your copyright notice in the Description page of Project Settings.


#include "FireWeapon.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFireWeapon::AFireWeapon()
{
	bReplicates = true;
	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
}

// Called when the game starts or when spawned
void AFireWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}




