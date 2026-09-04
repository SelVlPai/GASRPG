// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateMiniGame.h"
#include "AbilitySystemComponent.h"
#include "AttributeSets/AttributeSetCharacter.h"
#include "Net/UnrealNetwork.h"
APlayerStateMiniGame::APlayerStateMiniGame()
{
	SetNetUpdateFrequency(100.f);
	bReplicates = true;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAttributeSetCharacter>(TEXT("AttributeSet"));

}

UAbilitySystemComponent* APlayerStateMiniGame::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void APlayerStateMiniGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerStateMiniGame, TeamID);
	DOREPLIFETIME(APlayerStateMiniGame, Kills);
	DOREPLIFETIME(APlayerStateMiniGame, Deaths);
}


void APlayerStateMiniGame::SetTeamID(int32 NewTeamID)
{
	TeamID = NewTeamID;
}

bool APlayerStateMiniGame::IsSameTeam(APlayerStateMiniGame* OtherPlayerState)
{
	if (!OtherPlayerState) return false;
	return TeamID == OtherPlayerState->GetTeamID();
}

void APlayerStateMiniGame::Server_AddKills_Implementation()
{
	if (HasAuthority()) 
	{
		uint8 OldKills = Kills;
		Kills++;

		OnRep_Kills();
	}

}

void APlayerStateMiniGame::Server_AddDeaths_Implementation()
{
	if (HasAuthority())
	{
		uint8 OldDeaths = Deaths;
		Deaths++;

		OnRep_Deaths();
	}
}

void APlayerStateMiniGame::OnRep_TeamID()
{
}

void APlayerStateMiniGame::OnRep_Kills()
{
	BP_OnRep_Kills(Kills);
}

void APlayerStateMiniGame::OnRep_Deaths()
{
	BP_OnRep_Deaths(Deaths);
}
