// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include <AttributeSet.h>
#include "PlayerStateMiniGame.generated.h"


class UAttributeSetCharacter;
class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class GASRPG_API APlayerStateMiniGame : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:

	APlayerStateMiniGame();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSetCharacter* GetAttributeSet() const { return AttributeSet; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent)
	void AddScoreInTable(AController* KillerController,
		AController* VictimController);
	UFUNCTION(BlueprintCallable)
	int32 GetTeamID() const { return TeamID; };

	void SetTeamID(int32 NewTeamID);

	bool IsSameTeam(APlayerStateMiniGame* OtherPlayerState);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Replication")
	void Server_AddKills();
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Replication")
	void Server_AddDeaths();

private:
	UFUNCTION()
	void OnRep_TeamID();

protected:
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 TeamID = -1;

	UPROPERTY(ReplicatedUsing = OnRep_Kills, BlueprintReadWrite)
	uint8 Kills;
	UPROPERTY(ReplicatedUsing = OnRep_Deaths, BlueprintReadWrite)
	uint8 Deaths;

	UFUNCTION()
	void OnRep_Kills();
	UFUNCTION()
	void OnRep_Deaths();

	UFUNCTION(BlueprintImplementableEvent, Category = "Replication")
	void BP_OnRep_Kills(uint8 NewKills);
	UFUNCTION(BlueprintImplementableEvent, Category = "Replication")
	void BP_OnRep_Deaths(uint8 NewDeaths);

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSetCharacter> AttributeSet;

};
