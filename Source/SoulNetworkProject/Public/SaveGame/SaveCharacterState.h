// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SoulNetworkProject/ItemData.h"
#include "Classes/PickupActor.h"

#include "SaveCharacterState.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FSpawnData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	bool bRespawn = false;
	UPROPERTY()
	FName LastCheckPointName = NAME_None;
	UPROPERTY()
	FTransform LastTransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
};

 USTRUCT(BlueprintType)
 struct FRemainSoulData
 {
	 GENERATED_USTRUCT_BODY()

 public:
	UPROPERTY()
	int64 Soul = 0;
	UPROPERTY()
	FVector SpawnLocation = FVector::ZeroVector;
 };

 USTRUCT(BlueprintType)
 struct FWorldItem
 {
	 GENERATED_USTRUCT_BODY()

 public:
	UPROPERTY()
	TArray<FDataContent> Contents;

	UPROPERTY()
	FTransform WorldTransform;
 };

 USTRUCT(BlueprintType)
 struct FObjectData
 {
	 GENERATED_USTRUCT_BODY()

 public:
	UPROPERTY()
	FString ObjectName;
	UPROPERTY()
	TArray<bool> Activates;
 };

 USTRUCT(BlueprintType)
 struct FMapData
 {
	GENERATED_USTRUCT_BODY()

 public:
	UPROPERTY()
	FString WorldName;

	UPROPERTY()
	TArray<FObjectData> CheckPoints;

	UPROPERTY()
	TArray<FObjectData> BossMaangers;
	UPROPERTY()
	TArray<FObjectData> AIManagers;
	UPROPERTY()
	TArray<FObjectData> SmartObjects;

	UPROPERTY()
	TArray<FWorldItem> WorldItems;
	UPROPERTY()
	FRemainSoulData WorldRemainSoul;
 };
 

UCLASS()
class SOULNETWORKPROJECT_API USaveCharacterState : public USaveGame
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	bool SetCurrenWorldName(FString WorldName);
	UFUNCTION()
	bool SetupData(FString InName, TArray<FDataContent> InContents, uint8 MaxPt,uint8 MaxHealthPt, uint8 MaxManaPt);
	UFUNCTION()
	bool SetMapData(FString WorldName, class AActor* ObjectManager);

	//UFUNCTION()
	//bool SetWorldItems(FString WorldName, TArray<class APickupActor*> WorldItems);
	UFUNCTION()
	bool SetNewMapData(FString WorldName, TArray<FName> CheckNames);
	UFUNCTION()
	bool SetWorldItems(TArray<class APickupActor*> WorldItems);
	UFUNCTION()
	bool SetCheckPoints(TArray<FObjectData> CheckPointsData);
	UFUNCTION()
	bool SetBossManagers(TArray<FObjectData> BossManagersData);
	UFUNCTION()
	bool SetAIManagers(TArray<FObjectData> AIManagersData);
	UFUNCTION()
	bool SetSmartObjects(TArray<FObjectData> SmartObjectsData);


	//UFUNCTION()
	//void SetInvenWithStorageData(TArray<FDataContent> DataContents, TArray<FInventoryData> Storage)
	UFUNCTION()
	bool SetInvenIndex(uint8 InSpellIndex, uint8 InExpendableIndex, uint8 InL_WeaponIndex, uint8 InR_WeaponIndex);
	UFUNCTION()
	bool SetCondition(float InHealth, float InMana);
	UFUNCTION()
	bool SetStatus(int8 Level, int64 Soul);
	UFUNCTION()
	bool SetRemainSoul(int64 InSoul, FVector NewLocation);
	UFUNCTION()
	bool SetPotionData(FDataContent InHealthData, FDataContent InManaData);
	UFUNCTION()
	bool SetLastSpawnData(bool bRespawn, FTransform InLastTransform, FTransform InLastCPTransform);

	UFUNCTION()
	FString GetName() { return UserName; }
	UFUNCTION()
	TArray<FDataContent> GetContents() { return Contents; }
	UFUNCTION()
	void SetContents(TArray<FDataContent> InContents) { Contents = InContents; }
	UFUNCTION()
	uint8 GetMaxHealthPotion() const { return MaxHealthPotion; }
	UFUNCTION()
	uint8 GetCurrentHealthPotion() const { return CurrentHealthPotion; }
	UFUNCTION()
	uint8 GetMaxManaPotion() const { return MaxManaPotion; }
	UFUNCTION()
	uint8 GetCurrentManaPotion() const { return CurrentManaPotion; }
	UFUNCTION()
	uint8 GetUseLevel() const { return UserLevel; }
	UFUNCTION()
	FMapData GetMapData(FString RowName);

	UFUNCTION()
	FString GetFrontStackWorldName();
	UFUNCTION()
	FString GetLastStackWorldName();

public:
	UPROPERTY()
	FString UserName;
	UPROPERTY()
	uint8 UserLevel = 1;
	UPROPERTY()
	int32 UserSoul = 5000;

	UPROPERTY()
	FTransform LastTransform;

	UPROPERTY()
	float CurrentHealth = 100.0f;
	UPROPERTY()
	float CurrentMana = 100.0f;


	UPROPERTY()
	FDataContent HealthData;
	UPROPERTY()
	FDataContent ManaData;

	UPROPERTY()
	uint8 MaxPotionCount = 0;
	UPROPERTY()
	uint8 MaxHealthPotion = 0;
	UPROPERTY()
	uint8 MaxManaPotion = 0;
	UPROPERTY()
	uint8 CurrentHealthPotion = 0;
	UPROPERTY()
	uint8 CurrentManaPotion = 0;

	UPROPERTY()
	uint8 SpellIndex = 0;
	UPROPERTY()
	uint8 ExpendableIndex = 0;
	UPROPERTY()
	uint8 L_WeaponIndex = 0;
	UPROPERTY()
	uint8 R_WeaponIndex = 0;

	UPROPERTY()
	TArray<FDataContent> Contents;
	UPROPERTY()
	TArray<FInventoryData> StorageData;

	UPROPERTY()
	FString CurrentWorldName;
	UPROPERTY()
	TArray<FMapData> ActivateMaps;

	UPROPERTY()
	FSpawnData SpawnData;
};
