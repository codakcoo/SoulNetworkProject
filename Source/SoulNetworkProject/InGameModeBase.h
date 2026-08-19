// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulNetworkProjectGameModeBase.h"
//#include "SoulNetworkProject/SoulNetworkProjectGameModeBase"
#include "InGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AInGameModeBase : public ASoulNetworkProjectGameModeBase
{
	GENERATED_BODY()
	
public:

	//virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	//virtual bool SpawnPlayerFromSimulate(const FVector& NewLocation, const FRotator& NewRotation) override;

	virtual void Logout(AController* Exiting) override;

	UFUNCTION()
	void SetupOwnerLocation(APlayerController* NewPlayer);

protected:
	UFUNCTION()
	virtual void SpawnStartPoint(APlayerController* Con) override;
	UFUNCTION()
	virtual void SpawnCheckPoint(APlayerController* Con) override;

private:
	UFUNCTION()
	void UpdatedPlayerSpawn(APlayerController* NewPlayer);
	UFUNCTION()
	void BasicPlayerSpawn(APlayerController* NewPlayer);

	UFUNCTION()
	void PlayerSpawn(APlayerController* NewPlayer, FName SpawnTag);

};
