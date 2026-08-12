// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SoulNetworkProject/ItemData.h"
#include "SaveGame/SaveCharacterState.h"
#include "SaveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API ISaveInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void AddSaveData(TArray<FDataContent> DataContents, uint8 UserLevel, FString UserName, FTransform LastTransform) = 0;
	UFUNCTION()
	virtual void AddSaveWorldData(class AActor* ObjectManager,TArray<AActor*> WorldItems) = 0;
	UFUNCTION()
	virtual void AddSavePlayer(class APlayerCharacter* InPlayer) = 0;
	UFUNCTION()
	virtual void AddSaveRemainSoulData(int64 InSoul, FVector SpawnLocation) = 0;

	UFUNCTION()
	virtual void SaveInvenWithStorage(TArray<FDataContent> DataContents, FInventoryData StorageData) = 0;
	UFUNCTION()
	virtual void RemoveInvenWithStorage(TArray<FDataContent> DataContents, FInventoryData StorageData) = 0;

	UFUNCTION()
	virtual void NewTravel(FString SaveName, uint8 JobIdnex) = 0;
	UFUNCTION()
	virtual void LoadTravel(FString SaveName) = 0;

	UFUNCTION()
	virtual void RemoveSaveData(FString SaveName) = 0;
	UFUNCTION()
	virtual void SaveAudioSettings(float MasterValue, float MusicValue, float SFXValue, float VoiceValue) = 0;
	
	// save world data//
	UFUNCTION()
	virtual void SaveWorldItems(TArray<AActor*> WorldItems) = 0;
	UFUNCTION()
	virtual void SaveCheckPoints(TArray<FObjectData> CheckPointsData) = 0;
	UFUNCTION()
	virtual void SaveBossManagers(TArray<FObjectData> BossManagersData) = 0;
	UFUNCTION()
	virtual void SaveAIManagers(TArray<FObjectData> AIManagersData) = 0;
	UFUNCTION()
	virtual void SaveSmartObjects(TArray<FObjectData> SmartObjectsData) = 0;
	/////////////////////
	
	UFUNCTION()
	virtual void SaveInventoryData(class UInventoryComponent* InventoryComponent) = 0;
	UFUNCTION()
	virtual void SaveInvenIndex(uint8 SpellIndex, uint8 ExpendableIndex, uint8 L_WeaponIndex, uint8 R_WeaponIndex) = 0;
	UFUNCTION()
	virtual void SaveCondition(float InHealth, float InMana) = 0;

	UFUNCTION()
	virtual void SaveRespawn(bool bRespawn) = 0;
	UFUNCTION()
	virtual void SaveLastTransform(FTransform LastTransform) = 0;
	UFUNCTION()
	virtual void SaveLastCheckPoint(FName PointName) = 0;

	UFUNCTION()
	virtual class USaveCharacterState* GetCurrentData() const = 0;
	UFUNCTION()
	virtual FString GetCurrnetSaveName() const = 0;
	UFUNCTION()
	virtual FString GetCurrentWorlName() const = 0;
	UFUNCTION()
	virtual bool GetNewMapData() const = 0;
	UFUNCTION()
	virtual UTexture2D* GetMapTexutre(FName RowName) = 0;
	UFUNCTION()
	virtual UTexture2D* GetPointTexture(FName RowName, FName PointName) = 0;
};
