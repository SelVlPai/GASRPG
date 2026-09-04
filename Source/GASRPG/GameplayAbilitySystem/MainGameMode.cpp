// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "MainGameState.h"
#include "PlayerStateMiniGame.h"

void AMainGameMode::BP_PlayerDead_Implementation(AController* VictimController, AController* KillerController)
{
	if (VictimController && KillerController) 
	{
		APlayerStateMiniGame* VictimmPS = VictimController->GetPlayerState<APlayerStateMiniGame>();
		APlayerStateMiniGame* KillerPS = KillerController->GetPlayerState<APlayerStateMiniGame>();
		if (KillerPS && VictimmPS)
		{
			VictimmPS->Server_AddDeaths_Implementation();
			KillerPS->Server_AddKills_Implementation();
			int32 KillerTeamScore;
			int32 ScoringTeam = KillerPS->GetTeamID();
			if (ScoringTeam == 0)
			{
				Team0Score++;
				KillerTeamScore = Team0Score;
			}
			else
			{
				Team1Score++;
				KillerTeamScore = Team1Score;
			}
			AMainGameState* MainGameState = GetGameState<AMainGameState>();
			if (!MainGameState) return;
			MainGameState->Multicast_UpdateTeamScore(Team0Score, Team1Score);
			if (KillerTeamScore >= TeamScoreToWin) 
			{
				EndGame = true;
				CompleteMatch();
				MainGameState->Multicast_OnTeamGameOver(ScoringTeam);

			}
		}

	}

}

void AMainGameMode::PostLogin(APlayerController* NewController)
{
	Super::PostLogin(NewController);
	AssignTeam(NewController);
}

void AMainGameMode::AssignTeam(APlayerController* PlayerController)
{
	if (!PlayerController) return;
	APlayerStateMiniGame* PlayerState = PlayerController->GetPlayerState<APlayerStateMiniGame>();
	if (!PlayerState) return;

	int32 AssignedTeam = 0;
	if (Team0Count > Team1Count) 
	{
		Team1Count++;
		AssignedTeam = 1;
	}
	else
	{
		Team0Count++;
		AssignedTeam = 0;
	}
	PlayerState->SetTeamID(AssignedTeam);

}
