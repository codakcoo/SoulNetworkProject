// Copyright Epic Games, Inc. All Rights Reserved.


#include "SoulNetworkProjectGameModeBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"
#include "Classes/ClothComponent.h"
#include "Classes/ObjectManager.h"
#include "SoulNetworkProject/Public/PvPPlayerState.h"

#include "SoulNetworkProjectGameInstance.h"
#include "SoulNetworkProjectGameStateBase.h"

#include "Kismet/GameplayStatics.h"

ASoulNetworkProjectGameModeBase::ASoulNetworkProjectGameModeBase()
{
	//set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass (TEXT("Blueprint'/Game/BP/Character/Player/LocalPawn_Bp.LocalPawn_Bp_C'"));
	if(PlayerPawnBPClass.Succeeded()) DefaultPawnClass = PlayerPawnBPClass.Class;

	bUseSeamlessTravel = true;
}

//void ASoulNetworkProjectGameModeBase::SaveWorldData_Implementation(APlayerController* NewPlayer)
//{
//	APlayerControllerBase* Con = Cast<APlayerControllerBase>(NewPlayer);
//
//	TArray<AActor*> WorldItems;
//	AActor* OM = UGameplayStatics::GetActorOfClass(GetWorld(), AObjectManager::StaticClass());
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickupActor::StaticClass(), WorldItems);
//
//	if (OM)
//	{
//		if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
//		{
//			Cast<ISaveInterface>(GetGameInstance())->AddSaveWorldData(OM, WorldItems);
//		}
//	}
//}

void ASoulNetworkProjectGameModeBase::JoinMessage(APlayerController* Con)
{
	if (APlayerControllerBase* PlayerCon = Cast<APlayerControllerBase>(Con))
	{
		FName Message = (TEXT("Enter in %s in room."), PlayerCon->GetPlayerName());
		AllPlayer_SendMessage(Message);
	}
}

void ASoulNetworkProjectGameModeBase::KillMessage(APlayerController* Con)
{
	if (APlayerControllerBase* PlayerCon = Cast<APlayerControllerBase>(Con))
	{
		FName Message;
		if (PlayerCon->GetLocalRole() == ROLE_Authority)
		{
			Message = TEXT("Host Player Dead. Leave the room.");
		}
		else
		{
			Message = TEXT("Remote Player Dead. Remote Player kick the room.");
		}
	}
}

void ASoulNetworkProjectGameModeBase::AllPlayer_SendMessage(FName Message)
{
	TArray<AActor*> Outers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerControllerBase::StaticClass(), Outers);

	for (auto& actor : Outers)
	{
		if (APlayerControllerBase* Con = Cast<APlayerControllerBase>(actor))
		{
			Con->BroadcastMessage(Message);
		}
	}
}

void ASoulNetworkProjectGameModeBase::SpawnStartLocation(APlayerController* Con)
{
	SpawnStartPoint(Con);
}

void ASoulNetworkProjectGameModeBase::SpawnCheckPointLocation(APlayerController* Con)
{
	SpawnCheckPoint(Con);
}

void ASoulNetworkProjectGameModeBase::AllPlayerRefreshCloth(APlayerController* Con)
{
	if (NumberOfPlayers == 0) return;

	TArray<AActor*> Outers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerControllerBase::StaticClass(), Outers);

	for (auto& actor : Outers)
	{
		if (actor == Con) continue;
		if (APlayerControllerBase* Control = Cast<APlayerControllerBase>(actor))
		{
			Control->BroadcastCloth();
		}
	}
}

void ASoulNetworkProjectGameModeBase::SendMessage(FName Message)
{
	TArray<AActor*> Outers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerControllerBase::StaticClass(), Outers);

	for (auto& actor : Outers)
	{
		if (APlayerControllerBase* Con = Cast<APlayerControllerBase>(actor))
		{
			Con->BroadcastMessage(Message);
		}
	}
}

//void ASoulNetworkProjectGameModeBase::SaveHostWorld()
//{
//	TArray<AActor*> OutActors;
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectManager::StaticClass(), OutActors);
//	if (AObjectManager* ObjManager = Cast<AObjectManager>(OutActors[0]))
//	{
//		ObjManager->SaveWorldObject();
//
//		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::White,__FUNCTION__);
//	}
//}
//
//void ASoulNetworkProjectGameModeBase::LoadHostWorld()
//{
//	if(!GetGameInstance()) return;
//	else if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
//	{
//		if(Cast<IMenuInterface>(GetGameInstance())->GetOpenSession()) return;
//
//		TArray<AActor*> OutActors;
//		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectManager::StaticClass(), OutActors);
//		if (AObjectManager* ObjManager = Cast<AObjectManager>(OutActors[0]))
//		{
//			ObjManager->LoadWorldObject();
//			//GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::White,__FUNCTION__);
//		}
//	}
//}