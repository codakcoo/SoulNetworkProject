// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "SoulNetworkProjectGameModeBase.h"
#include "SoulNetworkProjectGameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ALobbyGameMode : public ASoulNetworkProjectGameModeBase
{
	GENERATED_BODY()
	
public:
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

private:
	UFUNCTION()
	void StartGame();

	FTimerHandle InGameTimer;
	uint32 NumberOfPlayers = 0;
};
