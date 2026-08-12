// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoadInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULoadInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API ILoadInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//UFUNCTION()
	//virtual void LoadHostWorld() = 0;

	UFUNCTION()
	virtual void JoinMessage(APlayerController* Con) = 0;
	UFUNCTION()
	virtual void KillMessage(APlayerController* Con) = 0;
	UFUNCTION()
	virtual void SendMessage(FName Message) = 0;

	UFUNCTION()
	virtual void SpawnStartLocation(class APlayerController* Con) = 0;
	UFUNCTION()
	virtual void SpawnCheckPointLocation(class APlayerController* Con) = 0;
	UFUNCTION()
	virtual void AllPlayerRefreshCloth(class APlayerController* Con) = 0;
};
