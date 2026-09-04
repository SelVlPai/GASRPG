// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "Net/UnrealNetwork.h"

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Додаємо змінні для реплікації
	DOREPLIFETIME(AMainGameState, Team0Score);
	DOREPLIFETIME(AMainGameState, Team1Score);
}

void AMainGameState::Multicast_UpdateTeamScore_Implementation(int32 T0, int32 T1)
{
	Team0Score = T0;
	Team1Score = T1;
	UpdateTeamScore(Team0Score, Team1Score);
}

void AMainGameState::Multicast_OnTeamGameOver_Implementation(int32 WinningTeam)
{
	OnTeamGameOver(WinningTeam);
}

void AMainGameState::OnRep_TeamScores()
{
}
