// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API ICharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void SetupState(class APlayerCharacter* ApplyPawn) = 0;
	UFUNCTION()
	virtual void SetInventory(class APlayerCharacter* ApplyPawn) = 0;

	UFUNCTION()
	virtual void SetDeathEvent(class APlayerCharacter* ApplyPawn) = 0;

	UFUNCTION()
	virtual void SetVisibilityTimer(bool bActivate) = 0;
	UFUNCTION()
	virtual void UpdatedServerTimer(float InTimer) = 0;
	UFUNCTION()
	virtual void KickedPlayer() = 0;
	UFUNCTION()
	virtual void UpdatedWarpPoint() = 0;
	UFUNCTION()
	virtual void UpdatedCheckPoint() = 0;
	UFUNCTION()
	virtual void UpdatedRespawnPoint() = 0;

	UFUNCTION()
	virtual void SetupStartCharacter(class APlayerCharacter* ApplyPawn) = 0;
	UFUNCTION()
	virtual void SetupStartPoint(class APlayerCharacter* ApplyPawn) = 0;

	UFUNCTION()
	virtual void SetEnterBossArea(bool bActive) = 0;
	UFUNCTION()
	virtual bool GetEnterBossArea() = 0;
	UFUNCTION()
	virtual void StartBossAreaBroadcast() = 0;

	UFUNCTION()
	virtual void JoinEvnet() = 0;
	UFUNCTION()
	virtual void DeathEvent() = 0;
	UFUNCTION()
	virtual void ExitEvent() = 0;
};
