// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/ObjectManager.h"
#include "SoulNetworkProject/SoulNetworkProjectGameInstance.h"
#include "Classes/BossCharacter.h"
#include "Classes/EnemyCharacter.h"
#include "Classes/PickupActor.h"
#include "Classes/SmartObject/RemainSoul.h"
#include "Classes/AIManager.h"
#include "Classes/SmartObject.h"
#include "Classes/SmartObject/MovingPlatform.h"
#include "Classes/BossManager.h"
#include "Classes/SmartObject/BossEntrance.h"
#include "Classes/Interaction/SignPointActor.h"
#include "Classes/SmartObject/SignPointBlock.h"
#include "Classes/SmartObject/CheckPoint.h"
#include "SaveGame/SaveCharacterState.h"

#include "Kismet/GameplayStatics.h"
#include "SoulNetworkDebug.h"

// Sets default values
AObjectManager::AObjectManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AObjectManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AObjectManager::SaveWorldObject()
{
	//TArray<bool> AIActives;
	//TArray<bool> BossActives;
	//TArray<bool> WSActives;
	//TArray<AActor*> OutSpells;
	//TArray<AActor*> OutActors;
	//
	SoulDebug::ScreenLog(__FUNCTION__, FColor::Green, 10.f);

	TArray<FObjectData> CheckPointsData;
	TArray<FObjectData> AIManagersData;
	TArray<FObjectData> BossManagersData;
	TArray<FObjectData> SmartObjectsData;

	for(ACheckPoint* CheckPoint : CheckPoints)
	{
		FObjectData CheckData;
		CheckData.ObjectName = CheckPoint->PointName.ToString();
		CheckData.Activates.Add(CheckPoint->GetOriginalCondition());

		CheckPointsData.Add(CheckData);
	}
	for (AAIManager* AIManger : AIManagers)
	{
		FObjectData AIData;
		AIData.ObjectName = AIManger->GetName();
		if (AIManger->GetAgents().Num() == 0) continue;
		for (ACharacterBase* Agent : AIManger->GetAgents())
		{
			AIData.Activates.Add(Agent->GetDead());
		}
		AIManagersData.Add(AIData);
	}
	for (ABossManager* BossManager : BossManagers)
	{
		FObjectData AIData;
		AIData.ObjectName = BossManager->GetName();
		if(!BossManager->Boss) continue;
		else
		{
			AIData.Activates.Add(!BossManager->Boss->GetDead());
			AIData.Activates.Add(BossManager->GetShowedCinematic());
		}
			//AIActives.Add(Agent->GetDead());
		//BossActives.Add(BossManager->Boss->GetDead());
		BossManagersData.Add(AIData);
	}
	for (ASmartObject* SmartObject : SmartObjects)
	{
		FObjectData SOData;
		SOData.ObjectName = SmartObject->GetName();
		SOData.Activates.Add(SmartObject->GetOriginalCondition());

		SmartObjectsData.Add(SOData);
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickupActor::StaticClass(), OutActors);
	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		//Cast<ISaveInterface>(GetGameInstance())->AddSaveWorldData(AIActives, BossActives, WSActives, OutActors);
		Cast<ISaveInterface>(GetGameInstance())->SaveCheckPoints(CheckPointsData);
		Cast<ISaveInterface>(GetGameInstance())->SaveBossManagers(BossManagersData);
		Cast<ISaveInterface>(GetGameInstance())->SaveAIManagers(AIManagersData);
		Cast<ISaveInterface>(GetGameInstance())->SaveSmartObjects(SmartObjectsData);

		Cast<ISaveInterface>(GetGameInstance())->SaveWorldItems(OutActors);
	}
}

void AObjectManager::LoadWorldObject()
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		USaveCharacterState* SaveWorldData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData();
		if (SaveWorldData)
		{
			FMapData CurrnetMapData = SaveWorldData->GetMapData(Cast<ISaveInterface>(GetGameInstance())->GetCurrentWorlName());

			for (auto& ActiveData : CurrnetMapData.BossMaangers)
			{
				for (ABossManager* BossManager : BossManagers)
				{
					if (BossManager->GetName() == ActiveData.ObjectName)
					{
						BossManager->SetupManager(ActiveData.Activates[0], ActiveData.Activates[1]);
					}
				}
			}

			for (auto& ActiveData : CurrnetMapData.AIManagers)
			{

				for (AAIManager* AIManager : AIManagers)
				{
					if (AIManager->GetName() == ActiveData.ObjectName)
					{
						AIManager->SetupAgents(ActiveData.Activates);

						break;
					}
				}
			}

			for (auto& ActiveData : CurrnetMapData.CheckPoints)
			{
				for (ACheckPoint* CheckPoint : CheckPoints)
				{
					if (CheckPoint->PointName == FName(ActiveData.ObjectName))
					{
						CheckPoint->SetOriginalCondition(ActiveData.Activates[0]);

						break;
					}
				}
			}

			for (auto& ActiveData : CurrnetMapData.SmartObjects)
			{
				for (ASmartObject* SmartObject : SmartObjects)
				{
					if (SmartObject->GetName() == ActiveData.ObjectName)
					{
						SmartObject->SetOriginalCondition(ActiveData.Activates[0]);
						break;
					}
				}
			}

			if (PickupClass)
			{
				for (auto& Data : CurrnetMapData.WorldItems)
				{
					APickupActor* PA = GetWorld()->SpawnActor<APickupActor>(PickupClass, Data.WorldTransform);
					if (PA) PA->ContainContents(Data.Contents);
				}
			}
		
			if (RemainSoulClass)
			{
				if (CurrnetMapData.WorldRemainSoul.SpawnLocation != FVector::ZeroVector)
				{
					ARemainSoul* RS = GetWorld()->SpawnActor<ARemainSoul>(RemainSoulClass, CurrnetMapData.WorldRemainSoul.SpawnLocation, FRotator::ZeroRotator);
					if (RS) RS->SetSoul(CurrnetMapData.WorldRemainSoul.Soul);
				}
			}
		}
	}
}

void AObjectManager::AllActiavateForAM()
{
	for (auto& AIManager : AIManagers)
	{
		for (auto& Agent : AIManager->GetAgents())
		{
			Agent->SetupCharacter(true);
		}
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickupActor::StaticClass(), OutActors);
	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		//Cast<ISaveInterface>(GetGameInstance())->AddSaveWorldData(AIActives, BossActives, WSActives, OutActors);
		Cast<ISaveInterface>(GetGameInstance())->AddSaveWorldData(this, OutActors);
	}
}

void AObjectManager::RefreshManagers()
{
	for (auto& AIManager : AIManagers)
	{
		AIManager->RefreshAgents();
	}

	TArray<FObjectData> CheckPointsData;

	for (ACheckPoint* CheckPoint : CheckPoints)
	{
		FObjectData CheckData;
		CheckData.ObjectName = CheckPoint->PointName.ToString();
		CheckData.Activates.Add(CheckPoint->GetOriginalCondition());

		CheckPointsData.Add(CheckData);
	}

	Cast<ISaveInterface>(GetGameInstance())->SaveCheckPoints(CheckPointsData);
}

void AObjectManager::DeathEventBroadcastManager()
{
	for (AAIManager* AM : AIManagers)
	{
		AM->AllTransFalseforOnlyDeathValueInAgents();
	}
}

void AObjectManager::DeactivateInteractionObject()
{
	for (ABossManager* BM : BossManagers)
	{
		//BM->RefreshManager();


		//if(!BM->GetActivate()) continue;
		for (ABossEntrance* BE : BM->Entrances)
		{
			BE->SetShapeComponent(BE->TriggerBox, TEXT("NoCollision"));
		}
	}
	//for (AAIManager* AIManager : AIManagers)
	//{
	//	for (auto& Agent : AIManager->GetAgents())
	//	{
	//		Agent->SetupCharacter(Agent->GetDead());
	//	}
	//}

	for (ASignPointActor* SP : SignPoints)
	{
		//for (auto& Wall : SP->GetWalls())
		//{
		//	Wall->RefreshObject(true);
		//}
		SP->SetShapeComponent(SP->Collision, TEXT("NoCollision"));
	}
	for (ACheckPoint* CheckActor : CheckPoints)
	{
		CheckActor->SetShapeComponent(CheckActor->Collision, TEXT("NoCollision"));
	}
}

void AObjectManager::OpenSessionBroadCast()
{
	for (ABossManager* BM : BossManagers)
	{
		if (!BM->GetActivate()) continue;
		for (ABossEntrance* BE : BM->Entrances)
		{
			BE->SetShapeComponent(BE->TriggerBox, TEXT("NoCollision"));
		}
	}
}

void AObjectManager::ActiveBossManagers()
{
	for (ABossManager* BM : BossManagers)
	{
		if (!BM->GetActivate()) continue;
		for (ABossEntrance* BE : BM->Entrances)
		{
			BE->SetShapeComponent(BE->TriggerBox, TEXT("SmartObject"));
		}
	}
}

