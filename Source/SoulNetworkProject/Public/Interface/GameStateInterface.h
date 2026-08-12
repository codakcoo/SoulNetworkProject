// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "GameStateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API IGameStateInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void SetServerTimer(bool bActivate) = 0;
	UFUNCTION()
	virtual void CheckNumofPlayer(int8 NumofPlayer) = 0;
	UFUNCTION()
	virtual void DestroyServerReaction() = 0;

	UFUNCTION()
	virtual void SetCurrentBM(class ABossManager* EnterManager) = 0;
	UFUNCTION()
	virtual class ABossManager* GetCurrentBM() = 0;
	UFUNCTION()
	virtual void UpdatedAreaActorsAtBM() = 0;
	UFUNCTION()
	virtual void StartBossArea(class APlayerCharacter* EnterPawn) = 0;
	UFUNCTION()
	virtual void SetBossWidget(bool bVisible, APawn* ApplyPawn) = 0;
	UFUNCTION()
	virtual void CallEvnetUI(TArray<class ACharacterBase*> EventPawns, EEventUIType EventType) = 0;

	UFUNCTION()
	virtual void StartBossSequenceEvent() = 0;
	UFUNCTION()
	virtual void EndBossSequenceEvent() = 0;

	UFUNCTION()
	virtual void SkipCinematic() = 0;

	UFUNCTION()
	virtual void LoadWorldData() = 0;
	UFUNCTION()
	virtual void RefreshWorldData() = 0;
};
