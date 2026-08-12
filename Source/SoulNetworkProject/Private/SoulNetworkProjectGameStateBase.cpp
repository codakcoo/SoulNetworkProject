// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulNetworkProjectGameStateBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"
#include "Classes/ObjectManager.h"
#include "Classes/BossManager.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "SoulNetworkProject/InGameModeBase.h"

#include "Interface/CharacterInterface.h"
#include "Interface/MenuInterface.h"

#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

ASoulNetworkProjectGameStateBase::ASoulNetworkProjectGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASoulNetworkProjectGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ASoulNetworkProjectGameStateBase, bOpernServer);
}

void ASoulNetworkProjectGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	OriginServerTimer = OpenServerTimer;
	OriginStartBossTimer = OpenStartBossTimer;
}

void ASoulNetworkProjectGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bStartBossArea)
	{
		OpenStartBossTimer -= DeltaSeconds;
		//GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Cyan, FString::Printf(TEXT("%f"), OpenServerTimer));
		for (auto& PS : PlayerArray)
		{
			if (PS->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
			{
				Cast<ICharacterInterface>(PS)->UpdatedServerTimer(OpenStartBossTimer);
			}
		}
		if (OpenStartBossTimer <= 0.0f)
		{
			for (auto& PS : PlayerArray)
			{
				if (PS->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
				{
					Cast<ICharacterInterface>(PS)->SetVisibilityTimer(false);
				}
			}
			//Server_CloseServer();
			//SetServerTimer(false);
			SetBossArea(false);
			KickLostPlayerInBossArea();
		}
	}
	else if (bOpernServer)
	{
		OpenServerTimer -= DeltaSeconds;
		//GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Cyan, FString::Printf(TEXT("%f"), OpenServerTimer));
		for (auto& PS : PlayerArray)
		{
			if (PS->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
			{
				Cast<ICharacterInterface>(PS)->UpdatedServerTimer(OpenServerTimer);
			}
		}
		if (OpenServerTimer <= 0.0f)
		{
			for (auto& PS : PlayerArray)
			{
				if (PS->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
				{
					Cast<ICharacterInterface>(PS)->SetVisibilityTimer(false);
				}
			}
			Server_CloseServer();
			SetServerTimer(false);
		}
	}
}

void ASoulNetworkProjectGameStateBase::SetServerTimer(bool bActivate)
{
	if(bOpernServer == bActivate) return;

	bOpernServer = bActivate;
	//Server_SetServerTime(bActivate);
	for (auto& PS : PlayerArray)
	{
		if (PS->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			Cast<ICharacterInterface>(PS)->SetVisibilityTimer(bOpernServer);
		}
	}
	if(!bOpernServer)
	{
		OpenServerTimer = OriginServerTimer;
		AObjectManager* OM = Cast<AObjectManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AObjectManager::StaticClass()));
		if (OM)
		{
			OM->ActiveBossManagers();
		}
	}
	else
	{
		AObjectManager* OM = Cast<AObjectManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AObjectManager::StaticClass()));
		if (OM)
		{
			OM->OpenSessionBroadCast();
		}
	}
}

void ASoulNetworkProjectGameStateBase::CheckNumofPlayer(int8 NumofPlayer)
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		if(NumofPlayer == 1) Cast<IMenuInterface>(GetGameInstance())->Close();
		else if (NumofPlayer > 3) Cast<IMenuInterface>(GetGameInstance())->StartSession();
	}
}

void ASoulNetworkProjectGameStateBase::DestroyServerReaction()
{
	for (auto& PS : PlayerArray)
	{
		if (PS->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			Cast<ICharacterInterface>(PS)->KickedPlayer();
		}
	}
}

void ASoulNetworkProjectGameStateBase::SetCurrentBM(ABossManager* EnterManager)
{
	CurrentBM = EnterManager;
}

ABossManager* ASoulNetworkProjectGameStateBase::GetCurrentBM()
{
	return CurrentBM;
}

void ASoulNetworkProjectGameStateBase::UpdatedAreaActorsAtBM()
{
	if(!CurrentBM) return;
	TArray<class ACharacterBase*> HasInActors;
	for (auto& PS : PlayerArray)
	{
		if(PS->GetPawn()) HasInActors.Add(Cast<ACharacterBase>(PS->GetPawn()));
		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Yellow, PS->GetPawn() ? TEXT("HasPawn") : TEXT("NotPawn"));
	}
	CurrentBM->CheckAreaActors(HasInActors);
}

void ASoulNetworkProjectGameStateBase::StartBossArea(APlayerCharacter* EnterPawn)
{
	if (EnterPawn)
	{
		if (EnterPawn->GetLocalRole() == ROLE_Authority)
		{
			if (Cast<IMenuInterface>(GetGameInstance())->GetStartSession())
			{
				if (!GetWorld()->GetAuthGameMode()) return;
				else if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(ULoadInterface::StaticClass()))
				{
					SetBossArea(true);
					Cast<ILoadInterface>(GetWorld()->GetAuthGameMode())->SendMessage(TEXT("Start boss area."));
				}
				for (auto& PS : PlayerArray)
				{
					if (PS->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
					{
						Cast<ICharacterInterface>(PS)->StartBossAreaBroadcast();	
					}
				}
			}
		}
		if(!EnterPawn->GetPlayerState()) return;
		else if (EnterPawn->GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			Cast<ICharacterInterface>(EnterPawn->GetPlayerState())->SetEnterBossArea(true);
		}
	}
}

void ASoulNetworkProjectGameStateBase::SetBossWidget(bool bVisible, APawn* ApplyPawn)
{
	for (auto& PS : PlayerArray)
	{
		if(APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetPawn()->GetController()))
		if (PC)
		{
			if (ApplyPawn)
			{
				if(ApplyPawn == PS->GetPawn())
					PC->Client_SetBossWidget(bVisible);
			}
			else
			{
				PC->Client_SetBossWidget(bVisible);
			}
		}
	}
}

void ASoulNetworkProjectGameStateBase::CallEvnetUI(TArray<ACharacterBase*> EventPawns, EEventUIType EventType)
{
	for (auto& EventPawn : EventPawns)
	{
		if(APlayerControllerBase* PC = Cast<APlayerControllerBase>(EventPawn->GetController()))
			PC->Client_CallEventUI(EventType);
	}
}

void ASoulNetworkProjectGameStateBase::StartBossSequenceEvent()
{
	for (auto& PS : PlayerArray)
	{
		if (APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetPawn()->GetController()))
			if (PC)
			{
				PC->SetCinematic(true);
			}
		
	}
}

void ASoulNetworkProjectGameStateBase::EndBossSequenceEvent()
{
	for (auto& PS : PlayerArray)
	{
		if (APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetPawn()->GetController()))
			if (PC)
			{
				PC->SetCinematic(false);
			}
	}

}

void ASoulNetworkProjectGameStateBase::SkipCinematic()
{
	Server_SkipCinematic();
}

void ASoulNetworkProjectGameStateBase::LoadWorldData()
{
	//if (!GetGameInstance()) return;
	//else if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	//{
	//	if (Cast<IMenuInterface>(GetGameInstance())->GetOpenSession()) return;
	//
	//	TArray<AActor*> OutActors;
	//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectManager::StaticClass(), OutActors);
	//	if (AObjectManager* ObjManager = Cast<AObjectManager>(OutActors[0]))
	//	{
	//		ObjManager->LoadWorldObject();
	//	}
	//}

	Server_LoadWorldData();
}

void ASoulNetworkProjectGameStateBase::RefreshWorldData()
{
	if (PlayerArray.Num() == 1)
	{
		if (PlayerArray[0]->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			Cast<ICharacterInterface>(PlayerArray[0])->UpdatedCheckPoint();

			FTimerHandle ObjectTimer;
			GetWorld()->GetTimerManager().SetTimer(ObjectTimer, FTimerDelegate::CreateWeakLambda(this, [this]
			{
				AObjectManager* OutActor = Cast<AObjectManager>(UGameplayStatics::GetActorOfClass(this->GetWorld(), AObjectManager::StaticClass()));
				if (OutActor)
				{
					OutActor->RefreshManagers();
				}
			}), 2.0f, false);
		}
	}
}

void ASoulNetworkProjectGameStateBase::SetBossArea(bool bActivate)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bStartBossArea = bActivate;

		for (auto& PS : PlayerArray)
		{
			if (PS->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
			{
				Cast<ICharacterInterface>(PS)->SetVisibilityTimer(bStartBossArea);
			}
		}
		if(!bStartBossArea) OpenStartBossTimer = OriginStartBossTimer;
	}
	else
	{
		Server_SetBossArea(bActivate);
	}
}

void ASoulNetworkProjectGameStateBase::KickLostPlayerInBossArea()
{
	for (auto& PlayerState : PlayerArray)
	{
		if (PlayerState->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			if (!Cast<ICharacterInterface>(PlayerState)->GetEnterBossArea())
			{
				Cast<ICharacterInterface>(PlayerState)->KickedPlayer();
			}
		}
	}
	if (!GetWorld()->GetAuthGameMode()) return;
	else if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(ULoadInterface::StaticClass()))
	{
		Cast<ILoadInterface>(GetWorld()->GetAuthGameMode())->SendMessage(TEXT("Kicks out players not in the boss area. Refresh that has boss current status."));
	}
}

void ASoulNetworkProjectGameStateBase::Server_SkipCinematic_Implementation()
{
	if (CurrentBM)
	{
		CurrentBM->EndSequence();
	}
}

void ASoulNetworkProjectGameStateBase::Server_SetBossArea_Implementation(bool bActivate)
{
	SetBossArea(bActivate);
}

void ASoulNetworkProjectGameStateBase::Server_CloseServer_Implementation()
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		//GEngine->AddOnScreenDebugMessage(-1,10.f, FColor::Green, FString::Printf(TEXT("%d"), PlayerArray.Num()));
		if(PlayerArray.Num() == 1) Cast<IMenuInterface>(GetGameInstance())->Close();
		else if(PlayerArray.Num() > 1) Cast<IMenuInterface>(GetGameInstance())->StartSession();


	}
}

void ASoulNetworkProjectGameStateBase::Server_LoadWorldData_Implementation()
{
	if (!GetGameInstance()) return;
	else if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		if (Cast<IMenuInterface>(GetGameInstance())->GetOpenSession())
		{
			//TArray<AActor*> OutActors;
			//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectManager::StaticClass(), OutActors);
			//if (AObjectManager* ObjManager = Cast<AObjectManager>(OutActors[0]))
			//{
			//	ObjManager->DeactivateInteractionObject();
			//}
			//Multicast_LoadWorldData();
			return;
		}

		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectManager::StaticClass(), OutActors);
		if (AObjectManager* ObjManager = Cast<AObjectManager>(OutActors[0]))
		{
			SoulDebug::ScreenLog(__FUNCTION__, FColor::White, 10.f);
			ObjManager->LoadWorldObject();
		}
	}
}

void ASoulNetworkProjectGameStateBase::Multicast_LoadWorldData_Implementation()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectManager::StaticClass(), OutActors);
	if (AObjectManager* ObjManager = Cast<AObjectManager>(OutActors[0]))
	{
		ObjManager->DeactivateInteractionObject();
	}
}
