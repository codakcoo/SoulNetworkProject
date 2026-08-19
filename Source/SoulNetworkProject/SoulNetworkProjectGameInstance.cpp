// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulNetworkProjectGameInstance.h"
#include "Public/Widget/MainMenu.h"
#include "Widget/Game/FadeScreen.h"
#include "SaveGame/ClientInfo.h"

#include "Classes/SceneCapturePawn.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"
#include "Classes/InventoryComponent.h"
#include "Classes/Interaction/SignPointActor.h"
#include "Classes/ObjectManager.h"

#include "Interface/GameStateInterface.h"
#include "Interface/CharacterInterface.h"

//#include "MoviePlayer.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "SoulNetworkDebug.h"


#define CLIENT_NAME FName("Client").ToString()
#define SAVE_NAME FName("Save").ToString()

#define CAPTUREONE TEXT("First")
#define CAPTURESEC TEXT("Second")
#define CAPTURETHR TEXT("Third")
#define CAPTUREFOU TEXT("Fourth")
#define CAPTUREFIV TEXT("Fifth")


const static FName SESSION_NAME = TEXT("Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");
const static FString PARENTFOLDER = TEXT("/Game/Maps/");


USoulNetworkProjectGameInstance::USoulNetworkProjectGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetClass_C (TEXT("WidgetBlueprint'/Game/Widget/MainWidget_BP.MainWidget_BP_C'"));
	if (MainMenuWidgetClass_C.Succeeded())
	{
		MainMenuWidgetClass = MainMenuWidgetClass_C.Class;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> ItemDt_Obj(TEXT("DataTable'/Game/BP/Pickup/DataTable/ItemData.ItemData'"));
	if (ItemDt_Obj.Succeeded())
	{
		ItemDt = ItemDt_Obj.Object;
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDt_Obj(TEXT("DataTable'/Game/BP/Pickup/DataTable/WeaponData.WeaponData'"));
	if (WeaponDt_Obj.Succeeded())
	{
		WeaponDt = WeaponDt_Obj.Object;
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> ClothDt_Obj(TEXT("DataTable'/Game/BP/Pickup/DataTable/ClothData.ClothData'"));
	if (ClothDt_Obj.Succeeded())
	{
		ClothDt = ClothDt_Obj.Object;
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> ExpendableDt_Obj(TEXT("DataTable'/Game/BP/Pickup/DataTable/ExpendableData.ExpendableData'"));
	if (ExpendableDt_Obj.Succeeded())
	{
		ExpendableDt = ExpendableDt_Obj.Object;
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> SpellDt_Obj(TEXT("DataTable'/Game/BP/Pickup/DataTable/SpellData.SpellData'"));
	if (SpellDt_Obj.Succeeded())
	{
		SpellDt = SpellDt_Obj.Object;
	}
}

void USoulNetworkProjectGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem)
	{
		UE_LOG(LogSoulNetwork, Warning, TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString());
		
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface) 
		{
			UE_LOG(LogSoulNetwork, Warning, TEXT("Found SessionInterface"));
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USoulNetworkProjectGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &USoulNetworkProjectGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USoulNetworkProjectGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USoulNetworkProjectGameInstance::OnJoinSessionComplete);
			SessionInterface->OnStartSessionCompleteDelegates.AddUObject(this, &USoulNetworkProjectGameInstance::OnStartSessionComplete);
			//SessionInterface->OnSessionFailureDelegates.AddUObject(this, &USoulNetworkProjectGameInstance::)
		}
	}
	else 
	{
		UE_LOG(LogSoulNetwork, Warning, TEXT("Not Found Subsystem"));
	}

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &USoulNetworkProjectGameInstance::OnNetworkFailure);

		//FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &USoulNetworkProjectGameInstance::BeginLoadingScreen);
		//FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USoulNetworkProjectGameInstance::EndLoadingScreen);
	}

	if (UGameplayStatics::DoesSaveGameExist(CLIENT_NAME, 0))
	{
		ClientData = Cast<UClientInfo>(UGameplayStatics::LoadGameFromSlot(CLIENT_NAME, 0));
	}
	else
	{
		ClientData = Cast<UClientInfo>(UGameplayStatics::CreateSaveGameObject(UClientInfo::StaticClass()));
		UGameplayStatics::SaveGameToSlot(ClientData, CLIENT_NAME, 0);
	}
	UpdateSaveArray();
	if (FadeScreenWidgetClass)
	{
		FadeScreenWidget = CreateWidget<UFadeScreen>(this, FadeScreenWidgetClass);
	}
}


UUserWidget* USoulNetworkProjectGameInstance::LoadMenu()
{
	MainMenuWidget = CreateWidget<UMainMenu>(this, MainMenuWidgetClass);
	if (!MainMenuWidget || !MainMenuWidgetClass) return nullptr;

	MainMenuWidget->Setup();

	return MainMenuWidget;
}

void USoulNetworkProjectGameInstance::StartSession()
{
	if(SessionInterface)
	{
		UE_LOG(LogSoulNetwork,Warning, TEXT("Call StartSession!!!!!!!!"));
		SessionInterface->StartSession(SESSION_NAME);	//Session�� �����ϸ� �̼����� ����Ʈ�� ������ �Ƚ�Ŵ//

		if (!GetWorld()->GetGameState()) return;
		if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
		{
			Cast<IGameStateInterface>(GetWorld()->GetGameState())->SetServerTimer(false);
		}
	}
}

void USoulNetworkProjectGameInstance::Host(FName ServerName)
{
	CurrentServerName = ServerName;
	if (SessionInterface)
	{
		CreateSession();
	}
}

void USoulNetworkProjectGameInstance::BackMainMenu()
{
	if( GetWorld()->GetTimerManager().IsTimerActive(FadeTimer)) return;

	if (FadeScreenWidget)
	{
		FadeScreenWidget->AddToViewport();
		FadeScreenWidget->SetVisibility(ESlateVisibility::Visible);
		FadeScreenWidget->PlayFadeout();
	}

	GetWorld()->GetTimerManager().SetTimer(FadeTimer, FTimerDelegate::CreateWeakLambda(this, [&]()
		{
			LoadingScreen([this]()->void { MainMenuTravel(); });
		}), 2.0f, false);
}

void USoulNetworkProjectGameInstance::Join(uint32 Index)
{
	APlayerController* Con = GetFirstLocalPlayerController();
	if (!Con) return;
	if (!SessionInterface || !SessionSearch) return;
	//if(MainMenuWidget) MainMenuWidget->Teardown();	//MenuWidget ����//

	CurrentSignPoint = nullptr;

	auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
	if (ExistingSession)
	{ 
		SessionInterface->DestroySession(SESSION_NAME);
	}

	if (FadeScreenWidget)
	{
		FadeScreenWidget->SetVisibility(ESlateVisibility::Visible);
		FadeScreenWidget->AddToViewport();
		FadeScreenWidget->PlayFadeout();
	}
	SessionInterface->DestroySession(SESSION_NAME);
	SessionInterface->CancelFindSessions();
	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void USoulNetworkProjectGameInstance::Close()
{
	auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
	if (ExistingSession)
	{
		SessionInterface->DestroySession(SESSION_NAME);

		if(!GetWorld()->GetGameState()) return;
		if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
		{
			Cast<IGameStateInterface>(GetWorld()->GetGameState())->SetServerTimer(false);
		}
	}
}

void USoulNetworkProjectGameInstance::RefreshServerList_Implementation()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch)
	{
		//SessionSearch->bIsLanQuery = true;	//���� ��Ī����
		SessionSearch->MaxSearchResults = 1000;	//���� Ž������
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);	//���ͳݼ��� ���Ͱ��� ����
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());							//���� ã��
		
		UE_LOG(LogSoulNetwork, Warning, TEXT("Starting Find Session"));
	}
}

void USoulNetworkProjectGameInstance::SearchServerList(FName SessionName)
{
	if (!CurrentSignPoint) return;
	CurrentSignPoint->SwitchSearchEffect(true);
	CurrentServerName = SessionName;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch)
	{
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") SessionSearch->bIsLanQuery = true;	//���� ��Ī
		else SessionSearch->bIsLanQuery = false;
		//SessionSearch->bIsLanQuery = true;	//���� ��Ī����
		SessionSearch->MaxSearchResults = 1000;	//���� Ž������
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);	//���ͳݼ��� ���Ͱ��� ����
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());							//���� ã��

		UE_LOG(LogSoulNetwork, Warning, TEXT("Starting Find Session"));
	}
}

void USoulNetworkProjectGameInstance::SaveAudioSettings(float MasterValue, float MusicValue, float SFXValue, float VoiceValue)
{
	ClientData->MasterVolumeValue = MasterValue;
	ClientData->MusicVolumeValue = MusicValue;
	ClientData->SFXVolumeValue = SFXValue;
	ClientData->VoiceVolumeValue = VoiceValue;

	UGameplayStatics::SaveGameToSlot(ClientData, CLIENT_NAME, 0);
}

UClientInfo* USoulNetworkProjectGameInstance::GetClient()
{
	return ClientData;
}

uint8 USoulNetworkProjectGameInstance::GetUserLevel(FString UserName)
{
	USaveCharacterState* CurrentData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(UserName, 0));
	
	return CurrentData ? CurrentData->UserLevel : 0;
}

bool USoulNetworkProjectGameInstance::GetOpenSession()
{
	auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
	return ExistingSession ? true : false;
}

bool USoulNetworkProjectGameInstance::GetStartSession()
{
	auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
	if(!ExistingSession) return false;

	return ExistingSession->SessionState == EOnlineSessionState::InProgress ? true : false;
}

FName USoulNetworkProjectGameInstance::GetCurrentServerName()
{
	return CurrentServerName;
}

void USoulNetworkProjectGameInstance::AddSaveData(TArray<FDataContent> DataContents, uint8 UserLevel, FString UserName, FTransform LastTransform)
{
	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	CurrentSaveData->Contents = DataContents;

	//GetWorld()

	UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
	//CurrentSaveData->
}
void USoulNetworkProjectGameInstance::AddSaveWorldData(AActor* ObjectManager, TArray<AActor*> WorldItems)
{
	if(CurrentSaveName == "") return;

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	if(AObjectManager* OM = Cast<AObjectManager>(ObjectManager))
	{
		USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));


		TArray<APickupActor*> CastWorldItems;
		for(auto& WorldItem : WorldItems) CastWorldItems.Add(Cast<APickupActor>(WorldItem));
		CurrentSaveData->SetWorldItems(CastWorldItems);
	}

}

void USoulNetworkProjectGameInstance::AddSavePlayer(APlayerCharacter* InPlayer)
{
	if(!InPlayer) return;

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	if(!CurrentSaveData) return;
	bool bSuccess = CurrentSaveData->SetStatus(InPlayer->GetLevel(), InPlayer->GetSoul());

	if (!bSuccess)
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}
}

void USoulNetworkProjectGameInstance::AddSaveRemainSoulData(int64 InSoul, FVector SpawnLocation)
{
	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	if(!CurrentSaveData) return;

	CurrentSaveData->SetRemainSoul(InSoul, SpawnLocation);
}

void USoulNetworkProjectGameInstance::SaveWorldItems(TArray<AActor*> WorldItems)
{
	if (CurrentSaveName == "") return;

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));


	TArray<APickupActor*> CastWorldItems;
	for (auto& WorldItem : WorldItems) CastWorldItems.Add(Cast<APickupActor>(WorldItem));
	CurrentSaveData->SetWorldItems(CastWorldItems);
}

void USoulNetworkProjectGameInstance::SaveCheckPoints(TArray<FObjectData> CheckPointsData)
{
	if (CurrentSaveName == "") return;

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	CurrentSaveData->SetCheckPoints(CheckPointsData);
}

void USoulNetworkProjectGameInstance::SaveBossManagers(TArray<FObjectData> BossManagersData)
{
	if (CurrentSaveName == "") return;

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	CurrentSaveData->SetBossManagers(BossManagersData);
}

void USoulNetworkProjectGameInstance::SaveAIManagers(TArray<FObjectData> AIManagersData)
{
	if (CurrentSaveName == "") return;

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	CurrentSaveData->SetAIManagers(AIManagersData);
}

void USoulNetworkProjectGameInstance::SaveSmartObjects(TArray<FObjectData> SmartObjectsData)
{
	if (CurrentSaveName == "") return;

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	CurrentSaveData->SetSmartObjects(SmartObjectsData);
}

void USoulNetworkProjectGameInstance::SaveInventoryData(UInventoryComponent* InventoryComponent)
{
	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	if(!InventoryComponent || !CurrentSaveData) return;

	CurrentSaveData->SetInvenIndex(
		InventoryComponent->GetSpellIndex(), 
		InventoryComponent->GetExpendableIndex(), 
		InventoryComponent->GetL_AttachIndex(), 
		InventoryComponent->GetR_AttachIndex());
		
	if(!InventoryComponent->GetItemForSameName(TEXT("HealthPotion")) 
		|| !InventoryComponent->GetItemForSameName(TEXT("ManaPotion"))) return;

	CurrentSaveData->SetPotionData(
		InventoryComponent->ChangeItemDataToContent(*InventoryComponent->GetItemForSameName(TEXT("HealthPotion"))), 
		InventoryComponent->ChangeItemDataToContent(*InventoryComponent->GetItemForSameName(TEXT("ManaPotion"))));
	CurrentSaveData->Contents = InventoryComponent->PermuteItemDataToContents();

	UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
}

void USoulNetworkProjectGameInstance::SaveInvenIndex(uint8 SpellIndex, uint8 ExpendableIndex, uint8 L_WeaponIndex, uint8 R_WeaponIndex)
{
	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	if (CurrentSaveData)
	{
		CurrentSaveData->SetInvenIndex(SpellIndex, ExpendableIndex, L_WeaponIndex, R_WeaponIndex);
	}
}

void USoulNetworkProjectGameInstance::SaveCondition(float InHealth, float InMana)
{
	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	if (!CurrentSaveData) return;

	CurrentSaveData->SetCondition(InHealth, InMana);
}

void USoulNetworkProjectGameInstance::SaveRespawn(bool bRespawn)
{
	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	if (!CurrentSaveData) return;
	CurrentSaveData->SpawnData.bRespawn = bRespawn;

	UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
}

void USoulNetworkProjectGameInstance::SaveLastTransform(FTransform LastTransform)
{
	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	if (!CurrentSaveData) return;
	CurrentSaveData->SpawnData.LastTransform = LastTransform;

	UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
}

void USoulNetworkProjectGameInstance::SaveLastCheckPoint(FName PointName)
{
	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	if (!CurrentSaveData) return;
	CurrentSaveData->SpawnData.LastCheckPointName = PointName;

	UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
}


void USoulNetworkProjectGameInstance::SaveInvenWithStorage(TArray<FDataContent> DataContents, FInventoryData StorageData)
{

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	CurrentSaveData->Contents = DataContents;

	StorageData.HandType = EEquipHand::None;
	StorageData.bEquip = false;
	StorageData.EquipIndex = 0;


	if(StorageData.ItemData.ItemType != EItemType::Expendable && StorageData.ItemData.ItemType != EItemType::Arrow)
	{
		StorageData.StackIndex = CurrentSaveData->StorageData.Num() - 1;
		CurrentSaveData->StorageData.Add(StorageData);
		
		UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
		return;
	}
	else
	{
		for (auto& Data : CurrentSaveData->StorageData)
		{
			if (Data.ItemData.ItemName == StorageData.ItemData.ItemName)
			{
				if(Data.ItemCount == 99) continue;
				if (Data.ItemCount + StorageData.ItemCount > 99)
				{
					int8 OverCount = Data.ItemCount + StorageData.ItemCount - 99;

					StorageData.StackIndex = CurrentSaveData->StorageData.Num() - 1;
					StorageData.ItemCount = OverCount;

					Data.ItemCount = 99;
					CurrentSaveData->StorageData.Add(StorageData);
					

					UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
					return;
				}
				else
				{
					Data.ItemCount += StorageData.ItemCount;

					UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
					return;
				}
			}
		}
	}
	StorageData.StackIndex = CurrentSaveData->StorageData.Num() - 1;
	CurrentSaveData->StorageData.Add(StorageData);
	
	//GetWorld()
	UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
}

void USoulNetworkProjectGameInstance::RemoveInvenWithStorage(TArray<FDataContent> DataContents, FInventoryData StorageData)
{

	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveName, 0))
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	CurrentSaveData->Contents = DataContents;
	//CurrentSaveData->StorageData.RemoveSingle(StorageData);


	if (StorageData.ItemData.ItemType != EItemType::Expendable && StorageData.ItemData.ItemType != EItemType::Arrow)
	{
		//StorageData.StackIndex = CurrentSaveData->StorageData.Num() - 1;
		CurrentSaveData->StorageData.RemoveSingle(StorageData);
	}
	else
	{
		//FInventoryData RemoveData = StorageData;
		TArray<FInventoryData> BackupDatas;	
		for (auto& Data : CurrentSaveData->StorageData)
		{
			if (Data.ItemData.ItemName == StorageData.ItemData.ItemName)
			{
				BackupDatas.Add(Data);
			}
		}

		if (BackupDatas.Num() == 1)
		{
			if (BackupDatas[0].ItemCount - StorageData.ItemCount == 0)
			{
				CurrentSaveData->StorageData.RemoveSingle(BackupDatas[0]);
				BackupDatas.Empty();
			}
			else
			{
				BackupDatas[0].ItemCount -= StorageData.ItemCount;
			}
		}
		else
		{
			if (BackupDatas.Last().ItemCount - StorageData.ItemCount == 0) CurrentSaveData->StorageData.RemoveSingle(BackupDatas.Last());
			else if (BackupDatas[BackupDatas.Num() - 1].ItemCount - StorageData.ItemCount > 0)
			{
				BackupDatas[BackupDatas.Num() - 1].ItemCount -= StorageData.ItemCount;
			}
			else
			{
				int8 overCount = BackupDatas.Last().ItemCount - StorageData.ItemCount;
				CurrentSaveData->StorageData.RemoveSingle(BackupDatas.Last());
				BackupDatas.RemoveSingle(BackupDatas.Last());
				BackupDatas.Last().ItemCount += overCount;
			}
		}
		for (auto& BackupData : BackupDatas)
		{
			for (auto& StData : CurrentSaveData->StorageData)
			{
				if (StData.ItemData.ItemName == StorageData.ItemData.ItemName)
				{
					if (StData.StackIndex == BackupData.StackIndex)
					{
						StData.ItemCount = BackupData.ItemCount;
						break;
					}
				}
			}
		}
	}

	CurrentSaveData->StorageData = RefreshStorageData(CurrentSaveData->StorageData);

	//GetWorld()
	UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveName, 0);
}


void USoulNetworkProjectGameInstance::NewTravel(FString SaveName, uint8 JobIdnex)
{
	
	if(!GetMapRowData(StartMapName)) return;
	if (UGameplayStatics::DoesSaveGameExist(SaveName, 0)) return;
	if (!GetWorld() || (JobIdnex == 0 || JobIdnex > 2) || GetWorld()->GetTimerManager().IsTimerActive(FadeTimer))
	{
		UE_LOG(LogSoulNetwork, Warning, TEXT("Could not create session"));
		return;
	}

	//Run AddSaveData//
	USaveCharacterState* SaveData = Cast<USaveCharacterState>(UGameplayStatics::CreateSaveGameObject(USaveCharacterState::StaticClass()));

	bool bSuccess = SaveData->SetupData(SaveName,
		JobIdnex == 1 ? StartWarriorData : StartWizardData,
		5,
		JobIdnex == 1 ? 4 : 1,
		JobIdnex == 1 ? 1 : 4);

	if (!bSuccess)
	{
		if (!GetWorld()) return;
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;


		Con->ConsoleCommand("EXIT");
	}
	CurrentSaveName = SaveName;
	//CurrentWorldName = StartMapName.ToString();
	ClientData->AddSavename(SaveName);
	NewStackMapData(StartMapName);
	SaveData->SetCurrenWorldName(StartMapName.ToString());


	if (FadeScreenWidget)
	{
		FadeScreenWidget->AddToViewport();
		FadeScreenWidget->SetVisibility(ESlateVisibility::Visible);
		FadeScreenWidget->PlayFadeout();
	}

	GetWorld()->GetTimerManager().SetTimer(FadeTimer, FTimerDelegate::CreateWeakLambda(this, [&SaveName, this]()
	{
		LoadingScreen([this]()->void { DungeonTravel(); });
	}), 2.0f, false);
}
void USoulNetworkProjectGameInstance::LoadTravel(FString SaveName)
{
	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(SaveName, 0));

	if (!GetWorld() || !UGameplayStatics::DoesSaveGameExist(SaveName, 0) || !CurrentSaveData || GetWorld()->GetTimerManager().IsTimerActive(FadeTimer))
	{
		UE_LOG(LogSoulNetwork, Warning, TEXT("Could not create session"));
		return;
	}
	CurrentSaveName = SaveName;

	if(FadeScreenWidget)
	{
		FadeScreenWidget->AddToViewport();
		FadeScreenWidget->SetVisibility(ESlateVisibility::Visible);
		FadeScreenWidget->PlayFadeout();
	}

	GetWorld()->GetTimerManager().SetTimer(FadeTimer, FTimerDelegate::CreateWeakLambda(this, [&SaveName, this]()
	{
		LoadingScreen([this]()->void{ DungeonTravel(); });
	}),2.0f, false);
}
void USoulNetworkProjectGameInstance::RemoveSaveData(FString RemoveName)
{
	//if (!UGameplayStatics::DoesSaveGameExist(RemoveName, 0)) return;
	
	//Run RemoveSaveData//
	UGameplayStatics::DeleteGameInSlot(RemoveName, 0);
	ClientData->RemoveSaveName(RemoveName);

	MainMenuWidget->SetSaveList();
	/////////////////////
}

void USoulNetworkProjectGameInstance::NewStackMapData(FName NewMapName)
{
	if(CurrentSaveName == "") return;

	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	
	if (CurrentSaveData)
	{
		if (FMapStruct* MapStruct = GetMapRowData(NewMapName))
		{
			TArray<FName> PointNames;
			for (auto& PointStruct : MapStruct->PointStructs)
			{
				PointNames.Add(PointStruct.PointName);
			}
			CurrentSaveData->SetNewMapData(NewMapName.ToString(), PointNames);
		}
	}
}

USaveCharacterState* USoulNetworkProjectGameInstance::GetCurrentData() const
{
	return Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
}

bool USoulNetworkProjectGameInstance::GetNewMapData() const
{
	if(CurrentSaveName == "") return false;
	USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
	if (!CurrentSaveData)
	{
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		Con->ConsoleCommand("EXIT");

		return false;
	}

	for (auto& MapData : CurrentSaveData->ActivateMaps)
	{
		if(MapData.WorldName == CurrentSaveData->CurrentWorldName) return false;
	}

	return true;
}

FString USoulNetworkProjectGameInstance::GetCurrnetSaveName() const
{
	return CurrentSaveName;
}
FString USoulNetworkProjectGameInstance::GetCurrentWorlName() const
{
	FString outputString;
	if(GetCurrentData() && GetCurrentData()->CurrentWorldName != "") outputString = GetCurrentData()->CurrentWorldName;

	return outputString;
}

void USoulNetworkProjectGameInstance::RefreshServerTravel()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(FadeTimer)) return;

	if (FadeScreenWidget)
	{
		FadeScreenWidget->AddToViewport();
		FadeScreenWidget->SetVisibility(ESlateVisibility::Visible);
		FadeScreenWidget->PlayFadeout();
	}

	GetWorld()->GetTimerManager().SetTimer(FadeTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		LoadingScreen([this]()->void { DungeonTravel(); });
	}), 2.0f, false);
}

UTexture2D* USoulNetworkProjectGameInstance::SetupCharacterImage(FString SaveName, uint8 SaveIndex)
{
	if (SaveName != TEXT("") && SaveIndex < 5)
	{
		USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(SaveName, 0));
		if(!CurrentSaveData) return nullptr;

		FName SelectTagName = NAME_None;
		switch (SaveIndex)
		{
			case 0:
				SelectTagName = CAPTUREONE;
				break;
			case 1:
				SelectTagName = CAPTURESEC;
				break;
			case 2:
				SelectTagName = CAPTURETHR;
				break;
			case 3:
				SelectTagName = CAPTUREFOU;
				break;
			case 4:
				SelectTagName = CAPTUREFOU;
				break;
			default:
				return nullptr;
		}

		TArray<AActor*> OutActors;
		//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASceneCapturePawn::StaticClass(), OutActors);
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ASceneCapturePawn::StaticClass(), SelectTagName, OutActors);
		for (AActor* OutActor : OutActors)
		{
			if (ASceneCapturePawn* CapturePawn = Cast<ASceneCapturePawn>(OutActor))
			{
				return CapturePawn->UpdatedMeshs(CurrentSaveData);
			}
		}
	}
	return nullptr;
}

FMapStruct* USoulNetworkProjectGameInstance::GetMapRowData(FName RowName)
{
	if (Map_Dt)
	{
		FMapStruct* RowData = FindRow<FMapStruct>(Map_Dt, RowName, TEXT("Context Map Data."));
		
		return RowData ? RowData : nullptr;
	}
	return nullptr;
}


void USoulNetworkProjectGameInstance::SetSignPoint(ASignPointActor* InPointActor)
{
	CurrentSignPoint = InPointActor;
}

ASignPointActor* USoulNetworkProjectGameInstance::GetSignPoint() const
{
	return CurrentSignPoint;
}

void USoulNetworkProjectGameInstance::ScreenFade(bool bFadeIn)
{
	if(!FadeScreenWidget) FadeScreenWidget = CreateWidget<UFadeScreen>(this, FadeScreenWidgetClass);
	if (FadeScreenWidget)
	{
		FadeScreenWidget->AddToViewport();
		if(bFadeIn)
		{
			FadeScreenWidget->SetVisibility(ESlateVisibility::Visible); 
			FadeScreenWidget->PlayFadein();
		}
		else
		{
			FadeScreenWidget->SetVisibility(ESlateVisibility::Visible); 
			FadeScreenWidget->PlayFadeout();
		}
	}
}



void USoulNetworkProjectGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!GetWorld() || !Success)
	{
		UE_LOG(LogSoulNetwork, Warning, TEXT("Could not create session"));
		return;
	}

	if (CurrentSignPoint)
	{
		CurrentSignPoint->SwitchSearchWall(true);
		CurrentSignPoint->SwitchSearchEffect(true);
	}
}

void USoulNetworkProjectGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	//CreateSession();
	if (CurrentSignPoint)
	{
		CurrentSignPoint->SwitchSearchWall(false);
		CurrentSignPoint->SwitchSearchEffect(false);
		CurrentSignPoint = nullptr;

		if(!GetWorld()->GetGameState()) return;
		if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
		{
			Cast<IGameStateInterface>(GetWorld()->GetGameState())->DestroyServerReaction();
		}
	}
}

//���� ã��//
void USoulNetworkProjectGameInstance::OnFindSessionComplete(bool Success)
{
	UE_LOG(LogSoulNetwork, Warning, TEXT("Call FindSessionComplete"));
	if (Success && SessionSearch && MainMenuWidget)
	{
		UE_LOG(LogSoulNetwork, Warning, TEXT("Finished Find Session"));

		TArray<FServerData> ServerNames;
		//TArray<FTestServerData> ServerNames;	//MenuWidget ����//
		//�׽�Ʈ�� ����//
		//FServerData Data2;
		//ServerNames.Add(Data2);
		//ServerNames.Add(Data2);
		//ServerNames.Add(Data2);
		//////////////////

		for (auto& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogSoulNetwork, Warning, TEXT("Found session name : %s"), *SearchResult.GetSessionIdStr());
			//FTestServerData Data;	//MenuWidget ����//
			FServerData Data;
			Data.Name = SearchResult.GetSessionIdStr();
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUser = SearchResult.Session.OwningUserName;
			

			FString CurrnetUserName;
			if (SearchResult.Session.SessionSettings.Get(CurrentServerName, CurrnetUserName))
			{
				Data.HostUser = CurrnetUserName;
				ServerNames.Add(Data);
			}
			else
			{
				Data.Name = "Could not find name.";
			}
		}
		if (ServerNames.Num() == 0)
		{
			CurrentServerName = NAME_None;
			return;
		}
		//if(MainMenuWidget) MainMenuWidget->SetServerList(ServerNames);	//MenuWidget ����//
		if(CurrentSignPoint) CurrentSignPoint->SetServerRow(ServerNames);
	}
}
/////////////

void USoulNetworkProjectGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	//LoadMenu();
	if(GetWorld()->GetTimerManager().IsTimerActive(TravelTimer)) return;

	GetWorld()->GetTimerManager().SetTimer(TravelTimer, FTimerDelegate::CreateWeakLambda(this, [&]
		{
			LoadingScreen([this]()->void{DungeonTravel();});
		}), 0.1f, false);
}

//���� ����//
void USoulNetworkProjectGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	APlayerController* Con = GetFirstLocalPlayerController();
	if(!Con || !SessionInterface) return;
	
	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogSoulNetwork, Warning, TEXT("Could not get connect string."));
		return;
	}
	//if (MainMenuWidget) MainMenuWidget->Teardown();
	//BackMainMenu();


	//ConBase->AllClearTimer();

	//Con->GetPawn()->Destroy();
	if(!Con->GetPawn()->GetPlayerState()) return;
	else if (Con->GetPawn()->GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		Cast<ICharacterInterface>(Con->GetPawn()->GetPlayerState())->JoinEvnet();
	}

	Con->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}
UTexture2D* USoulNetworkProjectGameInstance::GetMapTexutre(FName RowName)
{
	FMapStruct* MapData = GetMapRowData(RowName);

	return MapData ? MapData->LevelImage : nullptr;
}
UTexture2D* USoulNetworkProjectGameInstance::GetPointTexture(FName RowName, FName PointName)
{
	FMapStruct* MapData = GetMapRowData(RowName);

	UTexture2D* OutputTexture = nullptr;
	for (auto& Data : MapData->PointStructs)
	{
		if (Data.PointName == PointName)
		{
			OutputTexture = Data.PointImage;
			break;
		}
	}

	return OutputTexture;
}
void USoulNetworkProjectGameInstance::OnStartSessionComplete(FName SessionName, bool bSuccess)
{
	if (CurrentSignPoint)
	{
		CurrentSignPoint->SwitchSearchEffect(false);
		CurrentSignPoint = nullptr;
	}
}
//////////////

//���� �����//
void USoulNetworkProjectGameInstance::CreateSession()
{
	if (SessionInterface)
	{
		FOnlineSessionSettings SessionSettings;

		//���� ������ ���� ���Ұ�� ������ 0���̴�.//
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") SessionSettings.bIsLANMatch = true;	//���� ��Ī
		else SessionSettings.bIsLANMatch = false;
		SessionSettings.NumPublicConnections = 4;	//�÷��̾� ��
		SessionSettings.bShouldAdvertise = true;	//����
		//SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.Set(CurrentServerName, CurrentSaveName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		//SessionSettings.bUsesPresence = true;
		//SessionSettings.Set(SEARCH_KEYWORDS, FString("MyUniqueKeyword"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		////////////////////////////////////////////

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
		SessionInterface->UpdateSession(SESSION_NAME, SessionSettings, true);

		if(!GetWorld()->GetGameState()) return;
		if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
		{
			Cast<IGameStateInterface>(GetWorld()->GetGameState())->SetServerTimer(true);
		}
	}
	else
	{
		UE_LOG(LogSoulNetwork, Warning, TEXT("Session Interface is not valid"));
	}
}

void USoulNetworkProjectGameInstance::LoadingScreen(TFunction<void(void)> TravelFunction)
{
	APlayerController* Con = GetFirstLocalPlayerController();
	if (CurrentSaveName == TEXT("")) return;// || !GetMapRowData(FName(*CurrentWorldName)) || !Con) return;

	SessionInterface->DestroySession(SESSION_NAME);
	SessionInterface->CancelFindSessions();

	FString TravelPath = PARENTFOLDER;
	TravelPath += TEXT("LoadingLevel");
	TravelPath += TEXT("?listen");
	Con->ClientTravel(TravelPath, ETravelType::TRAVEL_Absolute);
	TravelFunction();
}

void USoulNetworkProjectGameInstance::RefreshClientData()
{
	if (ClientData)
	{
		for (auto& SaveName : ClientData->SaveNames)
		{
			if(!UGameplayStatics::DoesSaveGameExist(SaveName, 0)) ClientData->SaveNames.RemoveSingle(SaveName);
		}

		UGameplayStatics::SaveGameToSlot(ClientData, CLIENT_NAME, 0);
	}
}

void USoulNetworkProjectGameInstance::UpdatedMapActives()
{
	if (Map_Dt)
	{
		USaveCharacterState* CurrentSaveData = Cast<USaveCharacterState>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
		if (CurrentSaveData)
		{
			for (auto& RowName : Map_Dt->GetRowNames())
			{
				FMapStruct MapStruct = *GetMapRowData(RowName);
				for (auto& ActivateMap : CurrentSaveData->ActivateMaps)
				{
					if (ActivateMap.WorldName == RowName.ToString())
					{
						MapActivates.Add(true, MapStruct);
						break;
					}
				}
				MapActivates.Add(false, MapStruct);
			}
		}
	}
}

TArray<FInventoryData> USoulNetworkProjectGameInstance::RefreshStorageData(TArray<FInventoryData> ApplyStorage)
{
	for (auto& Data : ApplyStorage)
	{
		if (Data.ItemData.ItemType == EItemType::Expendable || Data.ItemData.ItemType == EItemType::Arrow)
		{
			if (Data.ItemCount == 0)
			{
				ApplyStorage.RemoveSingle(Data);
				RefreshStorageData(ApplyStorage);

				break;
			}
		}
	}
	return ApplyStorage;
}
void USoulNetworkProjectGameInstance::RefreshActives()
{
}
void USoulNetworkProjectGameInstance::DungeonTravel()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(TravelTimer)) return;

	GetWorld()->GetTimerManager().SetTimer(TravelTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	{
		USaveCharacterState* SaveData = GetCurrentData();
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con || !SaveData) return;


		SoulDebug::ScreenLog(SaveData->CurrentWorldName, FColor::Red, 10.f);

		FString TravelWorldName = SaveData->CurrentWorldName;
		if (TravelWorldName == "")
		{
			TravelWorldName = SaveData->GetFrontStackWorldName();
			if (TravelWorldName == "")
			{
				TravelWorldName = SaveData->GetLastStackWorldName();
				if(TravelWorldName == "")
				{
					TravelWorldName = StartMapName.ToString();
				}
			}
		}
		SaveData->SetCurrenWorldName(TravelWorldName);

		FInputModeGameOnly InputModeData;
		//InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);

		Con->SetInputMode(InputModeData);
		Con->bShowMouseCursor = false;

		//if (MainMenuWidget) MainMenuWidget->Teardown();
		FString TravelPath = PARENTFOLDER;
		TravelPath += TravelWorldName;
		TravelPath += TEXT("?listen");
		Con->ClientTravel(TravelPath, ETravelType::TRAVEL_Absolute);
	}), 2.0f, false);

}
void USoulNetworkProjectGameInstance::MainMenuTravel()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(TravelTimer)) return;

	GetWorld()->GetTimerManager().SetTimer(TravelTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	{
		APlayerController* Con = GetFirstLocalPlayerController();
		if (!Con) return;

		SessionInterface->DestroySession(SESSION_NAME);
		SessionInterface->CancelFindSessions();

		Con->ClientTravel("/Game/Maps/MainMenu.MainMenu", ETravelType::TRAVEL_Absolute);
	}), 2.0f, false);

}

void USoulNetworkProjectGameInstance::UpdatedSaveDataRenderTexture()
{
	//UGameplayStatics::GetAllActorsOfClass()
}

void USoulNetworkProjectGameInstance::UpdateSaveArray()
{
	if (ClientData)
	{
		for (auto& SaveName : ClientData->SaveNames)
		{
			if (!UGameplayStatics::DoesSaveGameExist(SaveName, 0))
			{
				ClientData->SaveNames.RemoveSingle(SaveName);
				UGameplayStatics::SaveGameToSlot(ClientData, CLIENT_NAME, 0);
				UpdateSaveArray();
				break;
			}
		}
	}
}
//////////////