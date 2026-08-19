// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ClientInfo.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API UClientInfo : public USaveGame
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void UpdateOptionValue();

	UFUNCTION()
	bool AddSavename(FString SaveName);
	UFUNCTION()
	bool RemoveSaveName(FString SaveName);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save")
	TArray<FString> SaveNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Option")
	float MasterVolumeValue = 0.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Option")
	float MusicVolumeValue = 0.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Option")
	float SFXVolumeValue = 0.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Option")
	float VoiceVolumeValue = 0.5f;

private:
	uint8 Capacity = 5;
};
