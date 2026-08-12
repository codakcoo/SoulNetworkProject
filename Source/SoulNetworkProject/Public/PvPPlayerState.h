// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PvPPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API APvPPlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:
	UFUNCTION(NetMulticast, Reliable)
	void AllPlayer_SendMessage(FName Message);
};
