// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GASRPG_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	void BP_PlayerDead(AController* VictimController, AController* KillerController);
	int32 Team0Score = 0;
	int32 Team1Score = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 TeamScoreToWin = 5;

	virtual void PostLogin(APlayerController* NewController) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void CompleteMatch();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Respawn = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 ScoreTowin = 10.f;
	UPROPERTY(BlueprintReadOnly)
	bool EndGame;
private:
	int32 Team0Count = 0;
	int32 Team1Count = 0;
	void AssignTeam(APlayerController* PlayerController);
	


};
