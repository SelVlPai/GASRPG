// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GASRPG/GameplayAbilitySystem/AttributeSets/AttributeSetCharacter.h"
#include "GASRPG/GameplayAbilitySystem/Framework/PlayerStateMiniGame.h"
#include "AbilitySystemComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject <UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 360.f, 0);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	AttributeSetCharacter = CreateDefaultSubobject<UAttributeSetCharacter>(TEXT("AttributeSetCharacter"));
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	BP_ChangeAttributesInWidget();
	FGameplayTag ParentTag = FGameplayTag::RequestGameplayTag(FName("State"));

	// Підписуємося на події додавання/видалення будь-яких тегів у гілці "State"
	AbilitySystemComponent->RegisterGenericGameplayTagEvent()
		.AddUObject(this, &AMainCharacter::OnGameplayTagChanged);
	if (HasAuthority())
	{
		
	}
}

void AMainCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Possessed"));
	FGameplayEffectQuery ClearAllEffects;
	//AbilitySystemComponent->RemoveActiveEffects(ClearAllEffects);
	//AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	//AbilitySystemComponent->ClearAllAbilities();
	InitAbilitySystemComponent();
	//AttributeSetCharacter = Cast<UAttributeSetCharacter>(AbilitySystemComponent->GetAttributeSet(UAttributeSetCharacter::StaticClass()));
	BP_InitAttribute();
	GiveAllAbilities();
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
		this, &AMainCharacter::Server_OnGameplayEffectAdded);
}

void AMainCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilitySystemComponent();
	if (HasActorBegunPlay()) 
	{
		BP_ChangeAttributesInWidget();
	}
	
}

void AMainCharacter::GiveAllAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent) return;

	for (TSubclassOf<UGameplayAbility>& AbilityClass : AllAbilitites) 
	{
		if (AbilityClass) 
		{
			FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, -1, this);
			AbilitySystemComponent->GiveAbility(AbilitySpec);
		}
	}
}


void AMainCharacter::Die()
{
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AMainCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMainCharacter::Server_OnGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	float Duration = Spec.GetDuration();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Possessed"));
	// Беремо Granted Tags або Dynamic Asset Tags ефекту
	FGameplayTagContainer TagContainer;
	Spec.GetAllGrantedTags(TagContainer);

	for (const FGameplayTag& Tag : TagContainer)
	{
		// Викликаємо Client RPC або Multicast, щоб передати Tag + Duration на всі клієнти
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State")))
			Multicast_SendBuffToUI(Tag, Duration);
	}
}

void AMainCharacter::Multicast_SendBuffToUI_Implementation(FGameplayTag Tag, float Duration)
{
	// Цей метод спрацює на всіх клієнтах, і ви зможете викликати ваш BP_OnBuffAdded(Tag, Duration)!
	BP_OnBuffAdded(Tag, Duration);
}

void AMainCharacter::OnGameplayTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State")))
	{
		if (NewCount > 0)
		{
			// Тег додався -> 'Tag' це точно State.Debuff.Poison!
			//BP_OnBuffAdded(Tag);
		}
	}

}

void AMainCharacter::InitAbilitySystemComponent()
{
	APlayerStateMiniGame* PS = GetPlayerState<APlayerStateMiniGame>();
	if (!PS) return;
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSetCharacter = PS->GetAttributeSet();

	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
}

