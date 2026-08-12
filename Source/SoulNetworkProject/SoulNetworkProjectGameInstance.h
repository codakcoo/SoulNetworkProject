// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/DataTable.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "SaveGame/SaveCharacterState.h"

#include "SoulNetworkProject/ItemData.h"
#include "Public/Interface/MenuInterface.h"
#include "Public/Interface/DataInterface.h"
#include "Public/Interface/SaveInterface.h"

#include "SoulNetworkProjectGameInstance.generated.h"

/**
 * 
 */
 USTRUCT(BlueprintType)
 struct FPointStruct
 {
	 GENERATED_USTRUCT_BODY()

 public:
	 UPROPERTY(EditAnywhere)
	 FName PointName;
	 UPROPERTY(EditAnywhere)
	 UTexture2D* PointImage;
 };

USTRUCT(BlueprintType)
struct FMapStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* LevelImage;
	UPROPERTY(EditAnywhere)
	TArray<FPointStruct> PointStructs;
};

UCLASS()
class SOULNETWORKPROJECT_API USoulNetworkProjectGameInstance : public UGameInstance, public IMenuInterface, public IDataInterface, public ISaveInterface
{
	GENERATED_BODY()
	
public:
	USoulNetworkProjectGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	UUserWidget* LoadMenu();

	//-------------------Interface Functions--------------------------//
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	UFUNCTION(BlueprintCallable)
	virtual void Host(FName ServerName) override;	
	UFUNCTION()
	virtual void Join(uint32 Index) override;
	UFUNCTION()
	virtual void Close() override;
	UFUNCTION()
	virtual void StartSession() override;

	UFUNCTION()
	virtual	void BackMainMenu() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void RefreshServerList();
	virtual void RefreshServerList_Implementation() override;
	UFUNCTION()
	virtual void RefreshServerTravel() override;


	UFUNCTION()
	virtual class UTexture2D* SetupCharacterImage(FString SaveName, uint8 SaveIndex) override;

	// Data Interface //
	UFUNCTION()
	virtual UDataTable* GetItemDataTable() { return ItemDt; }
	UFUNCTION()
	virtual UDataTable* GetWeaponDataTable() { return WeaponDt; }
	UFUNCTION()
	virtual UDataTable* GetClothDataTable() { return ClothDt; }
	UFUNCTION()
	virtual UDataTable* GetExpendableDataTable() { return ExpendableDt; }
	UFUNCTION()
	virtual UDataTable* GetSpellDataTable() { return SpellDt; }
	////////////////////////////////////////////////////////////////////

	UFUNCTION()
	virtual void SearchServerList(FName SessionName) override;
	UFUNCTION()
	virtual void SaveAudioSettings(float MasterValue, float MusicValue, float SFXValue, float VoiceValue) override;

	UFUNCTION()
	virtual class UClientInfo* GetClient() override;
	UFUNCTION()
	virtual uint8 GetUserLevel(FString UserName) override;

	UFUNCTION()
	virtual bool GetOpenSession() override;
	UFUNCTION()
	virtual bool GetStartSession() override;
	UFUNCTION()
	virtual FName GetCurrentServerName() override;

	UFUNCTION()
	virtual void SetSignPoint(class ASignPointActor* InPointActor) override;
	UFUNCTION()
	virtual class ASignPointActor* GetSignPoint() const override;

	UFUNCTION()
	virtual void ScreenFade(bool bFadeIn) override;

	UFUNCTION()
	virtual void NewTravel(FString SaveName, uint8 JobIdnex) override;
	UFUNCTION()
	virtual void LoadTravel(FString SaveName) override;

	UFUNCTION()
	virtual void AddSaveData(TArray<FDataContent> DataContents, uint8 UserLevel, FString UserName, FTransform LastTransform) override;
	UFUNCTION()
	virtual void AddSaveWorldData(class AActor* ObjectManager, TArray<AActor*> WorldItems) override;
	UFUNCTION()
	virtual void AddSavePlayer(class APlayerCharacter* InPlayer) override;
	UFUNCTION()
	virtual void AddSaveRemainSoulData(int64 InSoul, FVector SpawnLocation) override;

	// save world data//
	UFUNCTION()
	virtual void SaveWorldItems(TArray<AActor*> WorldItems) override;
	UFUNCTION()
	virtual void SaveCheckPoints(TArray<FObjectData> CheckPointsData) override;
	UFUNCTION()
	virtual void SaveBossManagers(TArray<FObjectData> BossManagersData) override;
	UFUNCTION()
	virtual void SaveAIManagers(TArray<FObjectData> AIManagersData) override;
	UFUNCTION()
	virtual void SaveSmartObjects(TArray<FObjectData> SmartObjectsData) override;
	/////////////////////

	UFUNCTION()
	virtual void SaveInventoryData(class UInventoryComponent* InventoryComponent) override;
	UFUNCTION()
	virtual void SaveInvenIndex(uint8 SpellIndex, uint8 ExpendableIndex, uint8 L_WeaponIndex, uint8 R_WeaponIndex) override;
	UFUNCTION()
	virtual void SaveCondition(float InHealth, float InMana) override;

	UFUNCTION()
	virtual void SaveRespawn(bool bRespawn) override;
	UFUNCTION()
	virtual void SaveLastTransform(FTransform LastTransform) override;
	UFUNCTION()
	virtual void SaveLastCheckPoint(FName PointName) override;

	UFUNCTION()
	virtual void SaveInvenWithStorage(TArray<FDataContent> DataContents, FInventoryData StorageData) override;
	UFUNCTION()
	virtual void RemoveInvenWithStorage(TArray<FDataContent> DataContents, FInventoryData StorageData) override;

	UFUNCTION()
	virtual void RemoveSaveData(FString SaveName) override;

	UFUNCTION()
	void NewStackMapData(FName NewMapName);

	UFUNCTION()
	virtual class USaveCharacterState* GetCurrentData() const override;
	UFUNCTION()
	virtual bool GetNewMapData() const override;
	UFUNCTION()
	virtual FString GetCurrnetSaveName() const override;
	UFUNCTION()
	virtual FString GetCurrentWorlName() const override;
	UFUNCTION()
	virtual UTexture2D* GetMapTexutre(FName RowName) override;
	UFUNCTION()
	virtual UTexture2D* GetPointTexture(FName RowName, FName PointName) override;

	void LoadingScreen(TFunction<void(void)> TravelFunction);


private:
	FMapStruct* GetMapRowData(FName RowName);

	void DungeonTravel();
	void MainMenuTravel();

	void UpdatedSaveDataRenderTexture();

public:
	UPROPERTY(EditDefaultsOnly)
	class UClientInfo* ClientData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FName StartMapName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* Map_Dt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<FDataContent> StartWarriorData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<FDataContent> StartWizardData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> LoadingScreenClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UMenuWidget> FadeScreenWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UFadeScreen* FadeScreenWidget;

private:
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnStartSessionComplete(FName SessionName, bool bSuccess);

	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void CreateSession();

	void RefreshClientData();

	void RefreshActives();

	void UpdateSaveArray();

	void UpdatedMapActives();

	TArray<FInventoryData> RefreshStorageData(TArray<FInventoryData> ApplyStorage);

	FString CurrentSaveName;
	FString CurrentWorldName;
	FName CurrentServerName;

	TSubclassOf<class UUserWidget> MainMenuWidgetClass;
	class UMainMenu* MainMenuWidget;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	class ASignPointActor* CurrentSignPoint = nullptr;

	class UDataTable* ItemDt;
	class UDataTable* WeaponDt;
	class UDataTable* ClothDt;
	class UDataTable* ExpendableDt;
	class UDataTable* SpellDt;

	TMap<bool, FMapStruct> MapActivates;
	FTimerHandle TravelTimer;
	FTimerHandle FadeTimer;
};
