// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameModeBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"
#include "Classes/ClothComponent.h"
#include "Classes/ObjectManager.h"
#include "Classes/SmartObject/StartPoint.h"
#include "Classes/Interaction/SignPointActor.h"
#include "SoulNetworkProject/Public/PvPPlayerState.h"
#include "SoulNetworkProjectPlayerState.h"

#include "GameFramework/PlayerStart.h"

#include "Interface/MenuInterface.h"
#include "Interface/GameStateInterface.h"
#include "Interface/CharacterInterface.h"

#include "SoulNetworkProjectGameInstance.h"
#include "SoulNetworkProjectGameStateBase.h"
#include "SoulNetworkLog.h"

#include "Kismet/GameplayStatics.h"

void AInGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	SetupOwnerLocation(NewPlayer);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, __FUNCTION__);
	//UpdatedPlayerSpawn(NewPlayer);

	APlayerControllerBase* Con = Cast<APlayerControllerBase>(NewPlayer);
	//APlayerCharacter* Chr = Cast<APlayerCharacter>(NewPlayer->GetPawn());
	if (Con)
	{
		if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
		{	
			//Cast<ISaveInterface>(GetGameInstance())->GetNewMapData() ? SaveHostWorld() : Con->LoadWorld();
			//LoadHostWorld();
			NumberOfPlayers++;

			if(!GetWorld()->GetGameState()) return;
			if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
			{
				Cast<IGameStateInterface>(GetWorld()->GetGameState())->LoadWorldData();
				Cast<IGameStateInterface>(GetWorld()->GetGameState())->CheckNumofPlayer(NumberOfPlayers);
			}
		}
	}
}

void AInGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	APlayerControllerBase* Con = Cast<APlayerControllerBase>(NewPlayer);
	if (Con)
	{
		Con->Server_SendJoinMessage();
	}
}

AActor* AInGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* StartPoint = nullptr;
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()) && 
		GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		if (Cast<IMenuInterface>(GetGameInstance())->GetOpenSession()) 
		{
			ASignPointActor* SignPoint = Cast<IMenuInterface>(GetGameInstance())->GetSignPoint();

			// SignPoint 가 없거나 Actor Tag 가 비어 있으면 기본 스폰으로 대체한다.
			// Tags[0] 을 검사 없이 읽으면 클라이언트 접속 시점에 어설션으로 크래시한다.
			if (!IsValid(SignPoint) || SignPoint->Tags.Num() == 0)
			{
				UE_LOG(LogSoulNetwork, Warning,
					TEXT("ChoosePlayerStart: SignPoint(%s) 에 Actor Tag 가 없어 기본 스폰으로 대체합니다."),
					IsValid(SignPoint) ? *SignPoint->GetName() : TEXT("None"));

				return FindPlayerStart(Player, TEXT("Level-Start-"));
			}

			FString TagName = SignPoint->Tags[0].ToString();
			TagName += FString::FromInt(NumberOfPlayers);
			StartPoint = FindPlayerStart(Player, TagName);

		}
		else
		{
			if (USaveCharacterState* SaveData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData())
			{
				
				TArray<AActor*> OutActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutActors);

				if (SaveData->SpawnData.bRespawn)
				{

					// 일치하는 지점을 찾으면 즉시 중단한다.
					// 매 반복마다 덮어쓰면 마지막 액터의 결과만 남아 체크포인트 리스폰이 실패한다.
					bool bFindSpawnSpot = false;
					for (AActor* OutActor : OutActors)
					{
						const APlayerStart* PlayerStart = Cast<APlayerStart>(OutActor);
						if (PlayerStart && PlayerStart->PlayerStartTag == SaveData->SpawnData.LastCheckPointName)
						{
							bFindSpawnSpot = true;
							break;
						}
					}
					bFindSpawnSpot ? 
						StartPoint = FindPlayerStart(Player, SaveData->SpawnData.LastCheckPointName.ToString()) : StartPoint = FindPlayerStart(Player, TEXT("Level-Start-"));

					//StartPoint = FindPlayerStart(Player, SaveData->SpawnData.LastCheckPointName.ToString());
				}
			}
		}
	}

	if (!StartPoint)
	{
		UE_LOG(LogSoulNetwork, Log, TEXT("ChoosePlayerStart: 지정 태그로 스폰 지점을 찾지 못해 Level-Start- 로 대체합니다."));
	}

	return StartPoint ? StartPoint : FindPlayerStart(Player, TEXT("Level-Start-"));
}


void AInGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	NumberOfPlayers--;
	if (!GetWorld()->GetGameState()) return;
	if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
	{
		Cast<IGameStateInterface>(GetWorld()->GetGameState())->CheckNumofPlayer(NumberOfPlayers);
		Cast<IGameStateInterface>(GetWorld()->GetGameState())->UpdatedAreaActorsAtBM();
	}
}

void AInGameModeBase::SetupOwnerLocation(APlayerController* NewPlayer)
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		if(Cast<IMenuInterface>(GetGameInstance())->GetOpenSession()) return;
		if (NewPlayer && NewPlayer->GetPawn())
		{
			if (USaveCharacterState* SaveData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData())
			{


			//if (ApplyPawn->UpdatedLastLocation().Z > SaveData->SpawnData.LastTransform.GetLocation().Z)
			//{
			//	FTransform SpawnTransform = FTransform(SaveData->SpawnData.LastTransform.GetRotation(), ApplyPawn->UpdatedLastLocation());
			//	ApplyPawn->SetActorTransform(SpawnTransform);
			//}
			//else
			//{
			//	ApplyPawn->SetActorTransform(SaveData->SpawnData.LastTransform);
			//}
				if(!SaveData->SpawnData.bRespawn && SaveData->SpawnData.LastTransform.GetLocation() != FVector::ZeroVector)
				NewPlayer->GetPawn()->SetActorTransform(SaveData->SpawnData.LastTransform);
			}
		}
	}
}

void AInGameModeBase::SpawnStartPoint(APlayerController* Con)
{
	if (Con)
	{
		BasicPlayerSpawn(Con);
	}
}

void AInGameModeBase::SpawnCheckPoint(APlayerController* Con)
{
	if (Con)
	{
		if(!GetGameInstance()) return;
		else if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
		{
			if (USaveCharacterState* CurrentData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData())
			{
				PlayerSpawn(Con, CurrentData->SpawnData.LastCheckPointName);
			}
		}
	}
}

void AInGameModeBase::UpdatedPlayerSpawn(APlayerController* NewPlayer)
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		
		if (Cast<IMenuInterface>(GetGameInstance())->GetOpenSession())
		{
			ASignPointActor* SignPoint = Cast<IMenuInterface>(GetGameInstance())->GetSignPoint();

			// ChoosePlayerStart 와 동일한 방어. 태그가 없으면 기본 스폰으로 넘긴다.
			if (!IsValid(SignPoint) || SignPoint->Tags.Num() == 0)
			{
				UE_LOG(LogSoulNetwork, Warning,
					TEXT("UpdatedPlayerSpawn: SignPoint(%s) 에 Actor Tag 가 없어 기본 스폰으로 대체합니다."),
					IsValid(SignPoint) ? *SignPoint->GetName() : TEXT("None"));

				BasicPlayerSpawn(NewPlayer);
				return;
			}

			FString TagName = SignPoint->Tags[0].ToString();
			TagName += FString::FromInt(NumberOfPlayers);

			if (NewPlayer->GetPawn()) NewPlayer->GetPawn()->Destroy();
			AActor* StartSpot = FindPlayerStart(NewPlayer, TagName);
			if (StartSpot)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, __FUNCTION__);
				APawn* NewPawn = SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetActorTransform());
				NewPlayer->Possess(NewPawn);
			}
		}
		else
		{
			APawn* CurrentPawn = NewPlayer->GetPawn();
			APlayerState* PS = CurrentPawn ? CurrentPawn->GetPlayerState() : nullptr;

			// 폰이나 PlayerState 가 아직 준비되지 않은 접속 타이밍에 대비한다.
			if (PS && PS->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
			{
				APlayerCharacter* Player = Cast<APlayerCharacter>(CurrentPawn);
				Cast<ICharacterInterface>(PS)->SetupStartPoint(Player);
			}
		}

	}
}

void AInGameModeBase::BasicPlayerSpawn(APlayerController* NewPlayer)
{
	FString TagName = TEXT("Level-Start-");

	if (NewPlayer->GetPawn()) NewPlayer->GetPawn()->Destroy();
	AActor* StartSpot = FindPlayerStart(NewPlayer, TagName);
	if (StartSpot)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, __FUNCTION__);
		APawn* NewPawn = SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetActorTransform());
		NewPlayer->Possess(NewPawn);
	}
}

void AInGameModeBase::PlayerSpawn(APlayerController* NewPlayer, FName SpawnTag)
{
	//FString TagName = TEXT("Level-Start-");

	if (NewPlayer->GetPawn()) NewPlayer->GetPawn()->Destroy();
	AActor* StartSpot = FindPlayerStart(NewPlayer, SpawnTag.ToString());
	if (StartSpot)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, __FUNCTION__);
		APawn* NewPawn = SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetActorTransform());
		NewPlayer->Possess(NewPawn);
	}
}
