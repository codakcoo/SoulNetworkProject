// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LoadInterface.h"
#include "SoulNetworkProjectGameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API ASoulNetworkProjectGameModeBase : public AGameModeBase, public ILoadInterface
{
	GENERATED_BODY()

public:
	ASoulNetworkProjectGameModeBase();

public:
	//UFUNCTION(Client, Reliable)
	//void SaveWorldData(class APlayerController* NewPlayer);

	UFUNCTION()
	virtual	void JoinMessage(class APlayerController* Con) override;
	UFUNCTION()
	virtual	void KillMessage(class APlayerController* Con) override;
	UFUNCTION()
	virtual void SendMessage(FName Message) override;
	
	UFUNCTION()
	virtual void SpawnStartLocation(class APlayerController* Con) override;
	UFUNCTION()
	virtual void SpawnCheckPointLocation(class APlayerController* Con) override;
	UFUNCTION()
	virtual void AllPlayerRefreshCloth(class APlayerController* Con) override;

	UFUNCTION()
	void AllPlayer_SendMessage(FName Message);

	//UFUNCTION()
	//void SaveHostWorld();
	//UFUNCTION()
	//virtual	void LoadHostWorld() override;

protected:
	virtual void SpawnStartPoint(APlayerController* Con) PURE_VIRTUAL(ASoulNetworkProjectGameModeBase, );
	virtual void SpawnCheckPoint(APlayerController* Con) PURE_VIRTUAL(ASoulNetworkProjectGameModeBase, );

protected:
	uint32 NumberOfPlayers = 0;

	FTimerHandle OutTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delay")
	float OutDelay = 5.0f;

};
