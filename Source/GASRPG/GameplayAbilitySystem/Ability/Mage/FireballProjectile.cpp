// Fill out your copyright notice in the Description page of Project Settings.


#include "FireballProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AFireballProjectile::AFireballProjectile()
{
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	bReplicates = true;
	SetReplicateMovement(true);

	InitialLifeSpan = LifeTime;
}

// Called when the game starts or when spawned
void AFireballProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}


