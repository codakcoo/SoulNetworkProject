// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulNetworkProjectPlayerState.h"
#include "Classes/PlayerControllerBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/InventoryComponent.h"
#include "Classes/EffectComponent.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/ObjectManager.h"
#include "Classes/BossManager.h"
#include "Classes/SmartObject/StartPoint.h"
#include "Classes/Interaction/SignPointActor.h"
#include "SaveGame/SaveCharacterState.h"

#include "Interface/SaveInterface.h"
#include "Interface/LoadInterface.h"
#include "Interface/MenuInterface.h"
#include "Interface/GameStateInterface.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameStateBase.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

#define TEAMUP TEXT("Teamup")
#define INVASION TEXT("Invasion")

ASoulNetworkProjectPlayerState::ASoulNetworkProjectPlayerState(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASoulNetworkProjectPlayerState::ClientInitialize(class AController* C)
{
	Super::ClientInitialize(C);
	
	//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Red,__FUNCTION__);
	//Client_StartCharacter();
	//Client_StartPoint();
}

void ASoulNetworkProjectPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoulNetworkProjectPlayerState, ControllerBase);
	DOREPLIFETIME(ASoulNetworkProjectPlayerState, PlayerIndex);
	DOREPLIFETIME(ASoulNetworkProjectPlayerState, bDead);
	DOREPLIFETIME(ASoulNetworkProjectPlayerState, MaxHealthPotionCount);
	DOREPLIFETIME(ASoulNetworkProjectPlayerState, MaxManaPotionCount);

	DOREPLIFETIME(ASoulNetworkProjectPlayerState, bEnterBossArea);
}



void ASoulNetworkProjectPlayerState::SetupState(APlayerCharacter* ApplyPawn)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, __FUNCTION__);

	USaveCharacterState* SaveData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData();
	if (SaveData && ApplyPawn && ApplyPawn->GetStateComponent())
	{
		ApplyPawn->GetStateComponent()->SetCurrentHealth(SaveData->CurrentHealth);
		ApplyPawn->GetStateComponent()->SetCurrentMana(SaveData->CurrentMana);
	}
}

void ASoulNetworkProjectPlayerState::SetInventory(APlayerCharacter* ApplyPawn)
{
}

void ASoulNetworkProjectPlayerState::SetDeathEvent(APlayerCharacter* ApplyPawn)
{
	if (ApplyPawn)
	{
		Client_DeathEvent();
	}
}

void ASoulNetworkProjectPlayerState::SetVisibilityTimer(bool bActivate)
{
	Client_SetVisibilityTimer(bActivate);
}
void ASoulNetworkProjectPlayerState::UpdatedServerTimer(float InTimer)
{
	Client_UpdatedTimer(InTimer);
}

void ASoulNetworkProjectPlayerState::KickedPlayer()
{
	KickedoftheServer();
}

void ASoulNetworkProjectPlayerState::UpdatedWarpPoint()
{
	if (!GetPawn() || !GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass())) return;
	else
	{
		APlayerControllerBase* Con = Cast<APlayerControllerBase>(GetPawn()->GetController());
		if (!Con) return;

		Con->Client_SavePlayerData();
		Server_FadeCharacterMesh(false);
		Con->DisableInput(Con);
		Cast<IMenuInterface>(GetGameInstance())->RefreshServerTravel();
	}
}

void ASoulNetworkProjectPlayerState::UpdatedCheckPoint()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(FadeTimer)) return;

	APlayerControllerBase* Con = Cast<APlayerControllerBase>(GetPawn()->GetController());
	if (GetPawn()->GetLocalRole() == ROLE_Authority && Con)
	{
		Con->Client_FadeScreen(false);

		GetWorld()->GetTimerManager().SetTimer(FadeTimer, FTimerDelegate::CreateWeakLambda(this, [this]
		{
			APlayerControllerBase* Con = Cast<APlayerControllerBase>(this->GetPawn()->GetController());
			if(Con)
			{
				Con->Client_FadeScreen(true);
				if (Con->GetPossessPawn() && Con->GetPossessPawn()->GetStateComponent())
				{
					Con->GetPossessPawn()->GetStateComponent()->SetupState();
				}
			}
			//if(!this->GetWorld()->GetGameState()) return;
			//if (this->GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
			//{
			//	Cast<IGameStateInterface>(this->GetWorld()->GetGameState())->RefreshWorldData();
			//}
		}), 2.0f, false);
	}
}

void ASoulNetworkProjectPlayerState::SetupStartCharacter(APlayerCharacter* ApplyPawn)
{
	Client_StartCharacter(ApplyPawn);
}

void ASoulNetworkProjectPlayerState::SetupStartPoint(APlayerCharacter* ApplyPawn)
{
	Client_StartPoint(ApplyPawn);
}

void ASoulNetworkProjectPlayerState::SetEnterBossArea(bool bActive)
{
	if (GetLocalRole() == ROLE_Authority) bEnterBossArea = bActive;
	else Server_SetEnterBossArea(bActive);
}

void ASoulNetworkProjectPlayerState::Server_SetEnterBossArea_Implementation(bool bActive)
{
	SetEnterBossArea(bActive);
}

bool ASoulNetworkProjectPlayerState::GetEnterBossArea()
{
	return bEnterBossArea;
}

void ASoulNetworkProjectPlayerState::StartBossAreaBroadcast()
{
	Client_StartBossArea();
}

void ASoulNetworkProjectPlayerState::UpdatedRespawnPoint()
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		Cast<ISaveInterface>(GetGameInstance())->SaveLastTransform(GetPawn()->GetActorTransform());
		SoulDebug::ScreenLog(__FUNCTION__, FColor::White, 10.f);

	}
}

void ASoulNetworkProjectPlayerState::JoinEvnet()
{
	Client_JoinEvent();
}

void ASoulNetworkProjectPlayerState::DeathEvent()
{
	Client_DeathEvent();
}

void ASoulNetworkProjectPlayerState::ExitEvent()
{
	Client_ExitEvent();
}

void ASoulNetworkProjectPlayerState::Client_JoinEvent_Implementation()
{
	APlayerControllerBase* Con = Cast<APlayerControllerBase>(GetPawn()->GetController());
	if(!Con) return;

	if (!GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass())) return;
	if (Con->GetPossessPawn())
	{
		FVector UpdatedLocation = Con->GetPossessPawn()->UpdatedLastLocation();
		if (UpdatedLocation == FVector::ZeroVector)
		{
			FVector LastLocation = Con->GetLastLocation() * Con->GetPossessPawn()->GetActorUpVector() * 10.0f;
			FTransform UpdatedTransform = FTransform(Con->GetPossessPawn()->GetActorRotation(), LastLocation);
			Cast<ISaveInterface>(GetGameInstance())->SaveLastTransform(UpdatedTransform);
		}
		else
		{
			FTransform UpdatedTransform = FTransform(Con->GetPossessPawn()->GetActorRotation(), Con->GetPossessPawn()->GetActorLocation());
			Cast<ISaveInterface>(GetGameInstance())->SaveLastTransform(UpdatedTransform);
		}

		AActor* OutActor = UGameplayStatics::GetActorOfClass(GetWorld(), AObjectManager::StaticClass());
		if (AObjectManager* ObjManager = Cast<AObjectManager>(OutActor))
		{
			Con->Client_SavePlayerData();
			ObjManager->SaveWorldObject();
			Con->Client_SaveCurrentCondition();
		}

		Con->DisableInput(Con);
	}
}
void ASoulNetworkProjectPlayerState::Client_DeathEvent_Implementation()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(FadeTimer)) return;

	APlayerControllerBase* Con = Cast<APlayerControllerBase>(GetPawn()->GetController());
	if (!Con) return;

	if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()) && 
		GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()) &&
		GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		Cast<IGameStateInterface>(GetWorld()->GetGameState())->CallEvnetUI({Cast<ACharacterBase>(GetPawn())}, EEventUIType::DEAD);
		if (GetPawn()->GetLocalRole() == ROLE_Authority)
		{
			Server_CallGMSendMessage(TEXT("Host Player Dead. Destroy Server Return to the origin server soon."));


			AActor* OutActor = UGameplayStatics::GetActorOfClass(GetWorld(), AObjectManager::StaticClass());
			if (AObjectManager* ObjManager = Cast<AObjectManager>(OutActor))
			{
				Cast<ISaveInterface>(GetGameInstance())->SaveRespawn(true);
				
				Con->Client_SaveLostSoul();
				Con->Client_SavePlayerData();
				Con->Client_SaveMaxCondition();
				ObjManager->DeathEventBroadcastManager();
				ObjManager->SaveWorldObject();
				//ObjManager->AllActiavateForAM();

				GetWorld()->GetTimerManager().SetTimer(FadeTimer, FTimerDelegate::CreateWeakLambda(this, [&]
				{
						Cast<IGameStateInterface>(GetWorld()->GetGameState())->DestroyServerReaction();
						Cast<IMenuInterface>(GetGameInstance())->RefreshServerTravel();
				}), 2.0f, false);
			}
		}
		else
		{
			Server_CallGMSendMessage(TEXT("Invasion Player Dead. Current Player has Return to the origin server soon."));
			Con->Client_SavePlayerData();

			GetWorld()->GetTimerManager().SetTimer(FadeTimer, FTimerDelegate::CreateWeakLambda(this, [&]
			{
				KickedoftheServer();
			}), 2.0f, false);
		}
	}
}

void ASoulNetworkProjectPlayerState::Client_ExitEvent_Implementation()
{
	APlayerControllerBase* Con = Cast<APlayerControllerBase>(GetPawn()->GetController());
	if(!Con) return;

	if(GetPawn()->GetLocalRole() == ROLE_Authority)
	{
		if (!GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()) || 
			!GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass())) return;
		if (Con->GetPossessPawn())
		{
			if (bEnterBossArea)
			{
				ABossManager* BM = Cast<IGameStateInterface>(GetWorld()->GetGameState())->GetCurrentBM();
				if (!BM) Cast<ISaveInterface>(GetGameInstance())->SaveRespawn(true);
				else
				{
					FVector LastLocation = BM->GetEnterancesSpawnLocation();
					FRotator LastRotation = BM->GetEnterancesSpawnRotation();


					if (LastLocation == FVector::ZeroVector) Cast<ISaveInterface>(GetGameInstance())->SaveRespawn(true);
					else
					{
						Cast<ISaveInterface>(GetGameInstance())->SaveRespawn(false);
						Cast<ISaveInterface>(GetGameInstance())->SaveLastTransform(FTransform(LastRotation, LastLocation));
					}
				}
			}
			else if (!bEnterBossArea)
			{
				FVector UpdatedLocation = Con->GetPossessPawn()->UpdatedLastLocation();
				if(UpdatedLocation == FVector::ZeroVector)
				{
					FVector LastLocation = Con->GetLastLocation() * Con->GetPossessPawn()->GetActorUpVector() * 10.0f;
					FTransform UpdatedTransform = FTransform(Con->GetPossessPawn()->GetActorRotation(), LastLocation);
					Cast<ISaveInterface>(GetGameInstance())->SaveLastTransform(UpdatedTransform);
				}
				else
				{
					FTransform UpdatedTransform = FTransform(Con->GetPossessPawn()->GetActorRotation(), Con->GetPossessPawn()->GetActorLocation());
					Cast<ISaveInterface>(GetGameInstance())->SaveLastTransform(UpdatedTransform);
				}
				Cast<ISaveInterface>(GetGameInstance())->SaveRespawn(false);
			}
		}

		Server_CallGMSendMessage(TEXT("Host Player Exit. Destroy Server Return to the origin server soon."));

		AActor* OutActor = UGameplayStatics::GetActorOfClass(GetWorld(), AObjectManager::StaticClass());
		if (AObjectManager* ObjManager = Cast<AObjectManager>(OutActor))
		{
			ObjManager->SaveWorldObject();
			Con->Client_SavePlayerData();
			Con->Client_SaveCurrentCondition();
		}
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Authority"));
	}
	else
	{
		Server_CallGMSendMessage(TEXT("Invasion Player Exit. Current Player Return to the origin server soon."));

		Con->Client_SavePlayerData();
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, TEXT("Authoproxy"));
	}
	Server_FadeCharacterMesh(false);
	Con->DisableInput(Con);
}

void ASoulNetworkProjectPlayerState::Client_SetVisibilityTimer_Implementation(bool bActivate)
{
	if (!GetPawn()) return;
	if (APlayerCharacter* ApplyPawn = Cast<APlayerCharacter>(GetPawn()))
	{
		if (ApplyPawn->GetHUD()) ApplyPawn->GetHUD()->SetServerTimer(bActivate);
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, __FUNCTION__);
	}
}

void ASoulNetworkProjectPlayerState::Client_UpdatedTimer_Implementation(float InTimer)
{
	if (!GetPawn()) return;
	if (APlayerCharacter* ApplyPawn = Cast<APlayerCharacter>(GetPawn()))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, __FUNCTION__);
		if(ApplyPawn->GetHUD()) ApplyPawn->GetHUD()->UpdateServerTimer(InTimer);
	}
}

void ASoulNetworkProjectPlayerState::Server_SpawnStartPointFromAuthGM_Implementation(APlayerCharacter* ApplyPawn)
{
	if(!ApplyPawn) return;

	if(!GetWorld()->GetAuthGameMode())return;
	if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(ULoadInterface::StaticClass()))
	{
		Cast<ILoadInterface>(GetWorld()->GetAuthGameMode())->SpawnStartLocation(Cast<APlayerController>(ApplyPawn->GetController()));
	}
}


void ASoulNetworkProjectPlayerState::SetControllerBase_Implementation(class APlayerControllerBase* Con)
{
	ControllerBase = Con;
}


void ASoulNetworkProjectPlayerState::SetPotionCount_Implementation(uint8 HealthCount, uint8 ManaCount)
{
	MaxHealthPotionCount = HealthCount;
	MaxManaPotionCount = ManaCount;

	//GEngine->AddOnScreenDebugMessage(1, 20.0f, FColor::Cyan, __FUNCTION__);
}



void ASoulNetworkProjectPlayerState::Server_SetupState_Implementation(APlayerCharacter* ApplyPawn)
{
	if (ApplyPawn)
	{
		if (ApplyPawn->GetLocalRole() == ROLE_Authority)
		{
			if (APlayerControllerBase* Con = Cast<APlayerControllerBase>(ApplyPawn->GetController()))
			{
				Con->Client_SetCurrentCondition();
			}
		}
	}
}

void ASoulNetworkProjectPlayerState::Server_SetInventory_Implementation(APlayerCharacter* ApplyPawn)
{
	if (ApplyPawn)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, __FUNCTION__);
		if (APlayerControllerBase* Con = Cast<APlayerControllerBase>(ApplyPawn->GetController()))
		{

		}
	}
}

void ASoulNetworkProjectPlayerState::Client_StartCharacter_Implementation(APlayerCharacter* ApplyPawn)
{
	if(!ApplyPawn) return;

	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		USaveCharacterState* SaveData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData();
		if (ApplyPawn->GetInventoryComponent() && SaveData)
		{
			ApplyPawn->GetInventoryComponent()->SetSpellIndex(SaveData->SpellIndex);
			ApplyPawn->GetInventoryComponent()->SetExpendableIndex(SaveData->ExpendableIndex);
			ApplyPawn->GetInventoryComponent()->SetL_AttachIndex(SaveData->L_WeaponIndex);
			ApplyPawn->GetInventoryComponent()->SetR_AttachIndex(SaveData->R_WeaponIndex);
			
			ApplyPawn->GetInventoryComponent()->SetMaxPotionsCount(SaveData->MaxPotionCount, SaveData->MaxHealthPotion, SaveData->MaxManaPotion);
			ApplyPawn->GetInventoryComponent()->SetPotionsData(SaveData->HealthData, SaveData->ManaData);
			if (SaveData->Contents.Num() != 0) ApplyPawn->GetInventoryComponent()->SetData(SaveData->Contents);
			ApplyPawn->GetInventoryComponent()->PermuteDataContentsToItemData();

		}

		if (ApplyPawn->GetLocalRole() != ROLE_Authority)
		{
			if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
			{
				FName ServerType = Cast<IMenuInterface>(GetGameInstance())->GetCurrentServerName();

				if(ServerType == TEAMUP) ApplyPawn->Client_SetFaction(EFaction::Neutral);
				else if(ServerType == INVASION) ApplyPawn->Client_SetFaction(EFaction::Enemy);
			}
		}
		StartSequence(ApplyPawn);
		//Server_StartSequence(ApplyPawn);
	}
}

void ASoulNetworkProjectPlayerState::Client_StartPoint_Implementation(APlayerCharacter* ApplyPawn)
{
	if(!ApplyPawn) return;
	else
	{
		if (ApplyPawn->GetLocalRole() == ROLE_Authority)
		{
			if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
			{
				USaveCharacterState* SaveData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData();
				if (!SaveData) return;

				if (SaveData->SpawnData.bRespawn)
				{
					if (SaveData->SpawnData.LastCheckPointName == NAME_None)
					{
						Server_SpawnStartPointFromAuthGM(ApplyPawn);
					}
					else
					{
						if(!GetWorld()->GetAuthGameMode()) return;
						if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(ULoadInterface::StaticClass()))
						{
							Cast<ILoadInterface>(GetWorld()->GetAuthGameMode())->SpawnCheckPointLocation(Cast<APlayerController>(ApplyPawn->GetController()));
						}
						//ApplyPawn->SetActorTransform(SaveData->SpawnData.LastCheckPointName);
					}
					Cast<ISaveInterface>(GetGameInstance())->SaveRespawn(false);
				}
				else
				{
					if (SaveData->SpawnData.LastTransform.GetLocation() == FVector::ZeroVector)
					{
						Server_SpawnStartPointFromAuthGM(ApplyPawn);
					}
					else
					{
						if (ApplyPawn->UpdatedLastLocation().Z > SaveData->SpawnData.LastTransform.GetLocation().Z)
						{
							FTransform SpawnTransform = FTransform(SaveData->SpawnData.LastTransform.GetRotation(), ApplyPawn->UpdatedLastLocation());
							ApplyPawn->SetActorTransform(SpawnTransform);
						}
						else
						{
							ApplyPawn->SetActorTransform(SaveData->SpawnData.LastTransform);
						}
					}
				}
			}
		}
	}
}

void ASoulNetworkProjectPlayerState::KickedoftheServer_Implementation()
{
	if (!GetPawn() || !GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass())) return;
	else
	{
		if (GetPawn()->GetLocalRole() != ROLE_Authority)
		{
			APlayerControllerBase* Con = Cast<APlayerControllerBase>(GetPawn()->GetController());
			if(!Con) return;

			Con->Client_SavePlayerData();
			Server_FadeCharacterMesh(false);
			Cast<IMenuInterface>(GetGameInstance())->RefreshServerTravel();
		}
	}
}

void ASoulNetworkProjectPlayerState::Client_StartBossArea_Implementation()
{
	if (AObjectManager* OM = Cast<AObjectManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AObjectManager::StaticClass())))
	{
		OM->ActiveBossManagers();
	}
}

void ASoulNetworkProjectPlayerState::Server_SetAlphaCharacterMesh_Implementation(APlayerCharacter* ApplyPawn, float Alpha)
{
	Multicast_SetAlphaCharacterMesh(ApplyPawn, Alpha);
}
void ASoulNetworkProjectPlayerState::Multicast_SetAlphaCharacterMesh_Implementation(APlayerCharacter* ApplyPawn, float Alpha)
{
	if (ApplyPawn)
	{
		ApplyPawn->GetEffectComponent()->AllMeshFadeout(Alpha);
		return;
	}

	//APlayerControllerBase* Con = Cast<APlayerControllerBase>(GetPawn()->GetController());
	//if (Con && Con->GetPossessPawn())
	//{
	//	ApplyPawn->GetEffectComponent()->AllMeshFadeout(Alpha);
	//}
}

void ASoulNetworkProjectPlayerState::Server_CallGMSendMessage_Implementation(FName Message)
{
	if(!GetWorld()->GetAuthGameMode()) return;
	else if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(ULoadInterface::StaticClass()))
	{
		Cast<ILoadInterface>(GetWorld()->GetAuthGameMode())->SendMessage(Message);
	}
}

void ASoulNetworkProjectPlayerState::Server_FadeCharacterMesh_Implementation(bool bFadeIn)
{
	Multicast_FadeCharacterMesh(bFadeIn);
}
void ASoulNetworkProjectPlayerState::Multicast_FadeCharacterMesh_Implementation(bool bFadeIn)
{
	APlayerControllerBase* Con = Cast<APlayerControllerBase>(GetPawn()->GetController());
	if(!Con) return;


	if (Con->GetPossessPawn())
	{
		Con->GetPossessPawn()->FadeInOut(bFadeIn);
	}
}

void ASoulNetworkProjectPlayerState::Server_StartSequence_Implementation(class APlayerCharacter* ApplyPawn)
{
	StartSequence(ApplyPawn);
}

void ASoulNetworkProjectPlayerState::StartSequence(APlayerCharacter* ApplyPawn)
{
	if(!ApplyPawn) return;

	if(ApplyPawn->GetPC()) ApplyPawn->GetPC()->Client_FadeScreen(true);
	if(ApplyPawn->GetLocalRole() != ROLE_Authority)
	{
		Server_SetAlphaCharacterMesh(ApplyPawn, 1.0f);
		//ApplyPawn->FadeInOut(false);
		ApplyPawn->StartMotion();

		AActor* OutActor = UGameplayStatics::GetActorOfClass(GetWorld(), AObjectManager::StaticClass());
		if (AObjectManager* OM = Cast<AObjectManager>(OutActor))
		{
			OM->DeactivateInteractionObject();
		}

		//Server_SetupWSObject();
	}
}
