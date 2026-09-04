// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameState.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API AMainGameState : public AGameStateBase
{
	GENERATED_BODY()
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_TeamScores, BlueprintReadOnly)
	int32 Team0Score = 0;
	UPROPERTY(ReplicatedUsing = OnRep_TeamScores, BlueprintReadOnly)
	int32 Team1Score = 0;
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateTeamScore(int32 T0, int32 T1);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnTeamGameOver(int32 WinningTeam);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTeamScore(int32 T0, int32 T1);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTeamGameOver(int32 WinningTeam);
private:
	UFUNCTION()
	void OnRep_TeamScores();
};