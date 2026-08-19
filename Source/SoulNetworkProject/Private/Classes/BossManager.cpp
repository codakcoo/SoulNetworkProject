// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/BossManager.h"
#include "Classes/BossCharacter.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/EffectComponent.h"
#include "Classes/SmartObject/BossEntrance.h"
#include "Classes/SmartObject/BossTrigger.h"
#include "Classes/SmartObject/BossStartPoint.h"
#include "Classes/SmartObject/EffectObject.h"
#include "Classes/SmartObject/MovingObject.h"
#include "Classes/SmartObject/CinematicObject.h"
#include "Classes/Interaction/SignPointActor.h"
#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"


#include "Interface/GameStateInterface.h"

#include "Components/BoxComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/GameStateBase.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

// Sets default values
ABossManager::ABossManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//LevelSequence = CreateDefaultSubobject<ULevelSequence>(TEXT("LevelSequence"));
	//LevelSequenceActor = CreateDefaultSubobject<ALevelSequenceActor>(TEXT("LevelSequenceActor"));

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABossManager::BeginPlay()
{
	Super::BeginPlay();

	//if(CinematicActor) CinematicActorLocation = CinematicActor->GetActorLocation();
	if (!Boss || Triggers.Num() == 0 || Entrances.Num() == 0)
	{
		UpdatedBoss(false);
		UpdatedTriggers(false);
		UpdatedEntrance(false);
	}

	if (SignPoint) SignPoint->RefreshObject(bActivate);
	if (!LevelSequence) bShowedCinematic = true;
	else
	{
		FMovieSceneSequencePlaybackSettings Settings;
		Settings.bAutoPlay = false;
		Settings.bPauseAtEnd = true;
		LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequence, Settings, LevelSequenceActor);

		Boss->SetActorLocation(GetShowedCinematicLocation());
	}

	UpdatedBoss(!bActivate);
	UpdatedTriggers(bActivate);
	for (ASmartObject* EventObject : EventObjects)
	{
		EventObject->SetupObject(!bActivate);
	}
	for (AEffectObject* EffectObject : EffectObjects)
	{
		if (!LevelSequencePlayer) EffectObject->SetupObject(true);
		EffectObject->SetupObject(bShowedCinematic);
		EffectObject->SetEffectOwner(Boss);
	}
	if (bActivate)
	{
		bShowedCinematic ? UpdatedEntrance(EnterType == EEnterType::Interaction ? true : false) : UpdatedEntrance(true);
	}
	else
	{
		UpdatedEntrance(false);
	}
	if (Boss->GetControllerBase())
	{
		Boss->GetControllerBase()->Manager = this;
	}

}

// Called every frame
void ABossManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABossManager, bActivate)
	DOREPLIFETIME(ABossManager, bShowedCinematic);
	DOREPLIFETIME(ABossManager, InAreaActors);
}

void ABossManager::SetupManager(bool bActive, bool bCinematic)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bActivate = bActive;
		bShowedCinematic = bCinematic;

		SetupObjects();
	}
	else
	{
		Server_SetupManager(bActive, bCinematic);
	}
}

void ABossManager::Server_SetupManager_Implementation(bool bActive, bool bCinemaic)
{
	SetupManager(bActive, bCinemaic);
}

void ABossManager::UpdatedBossManager()
{
	bActivate = !Boss->GetDead();

	UpdatedTriggers(bActivate);
	UpdatedEntrance(bActivate);
	if (SignPoint) SignPoint->RefreshObject(bActivate);
	for (ASmartObject* EventObject : EventObjects)
	{
		if(EventObject->GetClass()->ImplementsInterface(UEventInterface::StaticClass())) Cast<IEventInterface>(EventObject)->CallEvent();
	}
}

void ABossManager::UpdatedBoss(bool bActive)
{
	if(!Boss) return;
	Boss->SetManager(this);
	Boss->SetupCharacter(bActive);
}

void ABossManager::UpdatedEntrance(bool bActive)
{
	for (auto& Entrance : Entrances)
	{
		Entrance->SetEntranceWithFade(bActive);
	}
}

void ABossManager::UpdatedTriggers(bool bActive)
{
	for (auto& Trigger : Triggers)
	{
		Trigger->SetManager(this);
		Trigger->SetTrigger(true);
	}
}

FVector ABossManager::GetEnterancesSpawnLocation() const
{
	FVector SpawnLocation = FVector::ZeroVector;
	if (Boss && Triggers.Num() > 0 && Entrances.Num() > 0)
	{
		SpawnLocation = Entrances[0]->GetActorLocation() + Entrances[0]->LastSpawnLocation;
	}
	return SpawnLocation;
}

FRotator ABossManager::GetEnterancesSpawnRotation() const
{
	FRotator SpawnRotation = FRotator::ZeroRotator;
	if (Boss && Triggers.Num() > 0 && Entrances.Num() > 0)
	{
		SpawnRotation = UKismetMathLibrary::FindLookAtRotation(GetEnterancesSpawnLocation(), Entrances[0]->GetActorLocation());
		SpawnRotation.Pitch = 0.0f;
		SpawnRotation.Roll = 0.0f;
	}
	return SpawnRotation;
}

FVector ABossManager::GetShowedCinematicLocation() const
{
	FVector SpawnLocation = bShowedCinematic ? GetActorLocation() + ShowedCinematicLocation : Boss->GetActorLocation();
	return SpawnLocation;
}

void ABossManager::IncreaseAreaActor(ACharacterBase* ApplyPawn)
{
	if (Boss)
	{
		for(auto& InAreaActor : InAreaActors) if(ApplyPawn == InAreaActor) return;

		InAreaActors.Add(ApplyPawn);
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(ApplyPawn))
		{
			if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
			{
				Cast<IGameStateInterface>(GetWorld()->GetGameState())->SetBossWidget(true, Player);
				Player->Client_BindBossActor(Boss);

				if (InAreaActors.Num() == 1)
				{
					StartSequence(Player);
				}
			}
		}
	}
}

void ABossManager::DecreaseAreaActor(ACharacterBase* ApplyPawn)
{
	if (Boss)
	{
		InAreaActors.RemoveSingle(ApplyPawn);
		//if (InAreaActors.Num() == 1) Boss->StartBossStage(ApplyPawn);
	}
}

void ABossManager::CheckAreaActors(TArray<class ACharacterBase*> PermuteActors)
{
	SoulDebug::ScreenLog(FString::Printf(TEXT("%d"), InAreaActors.Num()), FColor::Purple, 10.f);

	TArray<ACharacterBase*> ExceptActors;
	for (auto& AreaActor : InAreaActors)
	{
		bool bExcept = true;
		for (auto& PermuteActor : PermuteActors)
		{
			if(AreaActor == PermuteActor)
			{
				bExcept = false;
				break;
			}
		}
		if(bExcept) ExceptActors.Add(AreaActor);
	}

	for (auto& ExceptActor : ExceptActors)
	{
		InAreaActors.RemoveSingle(ExceptActor);
	}
}

bool ABossManager::UpdatedAttackInManager(ACharacterBase* ApplyPawn)
{
	//if (ApplyPawn && InAreaActors.Num() > 0)
	{
		ApplyPawn->GetControllerBase()->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)ECombatState::Attack);
		ApplyPawn->GetControllerBase()->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
		ApplyPawn->GetControllerBase()->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);

		return true;
	}
	//return false;
}

void ABossManager::SingleEffect(bool bActive)
{
	if (bActive)
	{
		int8 Random = FMath::RandRange(0, EffectObjects.Num() - 1);
		EffectObjects[Random]->SetEffectObject(bActive);
	}
	else if (!bActive && CurrentEffectObject)
	{
		CurrentEffectObject->SetEffectObject(bActive);
	}
}

void ABossManager::MultiEffect(bool bActive)
{
	for (AEffectObject* EffectObject : EffectObjects)
	{
		EffectObject->SetEffectObject(bActive);
	}
}

void ABossManager::StartSequence(APlayerCharacter* InPlayer)
{
	StartBossStage(InPlayer);

	
	if(bShowedCinematic || !LevelSequencePlayer) Boss->GetControllerBase()->GetBrainComponent()->StartLogic();
	if (!bShowedCinematic && LevelSequencePlayer)
	{
		if(!GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass())) return;

		bShowedCinematic = true;
		LevelSequencePlayer->Play();
		Boss->GetControllerBase()->GetBrainComponent()->StopLogic("Agent is Dead");
		Boss->GetEffectComponent()->AllMeshFadeout(1.0f);
		Cast<IGameStateInterface>(GetWorld()->GetGameState())->StartBossSequenceEvent();
		//if(CinematicActor) CinematicActor->SetActorLocation(Boss->GetActorLocation());
		//StartLevelSequence.Broadcast();
	}
}

void ABossManager::EndSequence()
{
	if (!GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass())) return;

	LevelSequencePlayer->Stop();
	Boss->GetControllerBase()->GetBrainComponent()->StartLogic();
	Cast<IGameStateInterface>(GetWorld()->GetGameState())->EndBossSequenceEvent();
	//StartBossStage(Player);
	//if(CinematicActor) CinematicActor->SetActorLocation(CinematicActorLocation);
	//EndLevelSequence.Broadcast();
}

bool ABossManager::GetShowedCinematic() const
{
	if(!LevelSequence || bShowedCinematic) return true;

	return false;
}

void ABossManager::SetupObjects_Implementation()
{
	SoulDebug::ScreenLog(__FUNCTION__, FColor::Blue, 10.f);

	if (!Boss || Triggers.Num() == 0 || Entrances.Num() == 0)
	{
		UpdatedBoss(false);
		UpdatedTriggers(false);
		UpdatedEntrance(false);

	}

	if (SignPoint) SignPoint->RefreshObject(bActivate);
	if (!LevelSequence) bShowedCinematic = false;
	else
	{
		FMovieSceneSequencePlaybackSettings Settings;
		Settings.bAutoPlay = false;
		Settings.bPauseAtEnd = true;
		LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequence, Settings, LevelSequenceActor);

		Boss->SetActorLocation(GetShowedCinematicLocation());
	}

	UpdatedBoss(!bActivate);
	UpdatedTriggers(bActivate);
	for (ASmartObject* EventObject : EventObjects)
	{
		EventObject->SetupObject(!bActivate);
	}
	for (AEffectObject* EffectObject : EffectObjects)
	{
		if (!LevelSequencePlayer) EffectObject->SetupObject(true);
		EffectObject->SetupObject(bShowedCinematic);
		EffectObject->SetEffectOwner(Boss);
	}
	if (bActivate)
	{
		bShowedCinematic ? UpdatedEntrance(EnterType == EEnterType::Interaction ? true : false) : UpdatedEntrance(true);
	}
	else
	{
		UpdatedEntrance(false);
	}
	if (Boss->GetControllerBase())
	{
		Boss->GetControllerBase()->Manager = this;
	}
}

void ABossManager::StartBossStage_Implementation(APlayerCharacter* Player)
{
	if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
	{
		if (!bShowedCinematic)
		{
			Boss->GetControllerBase()->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Attack);
		}
		else
		{
			if (LevelSequencePlayer)
			{
				LevelSequencePlayer->Play();
			}
		}
		if (Boss->GetControllerBase())
		{
			if (!Boss->GetControllerBase()->GetBlackboard()->GetValueAsObject(bb_keys::target_actor))
			{
				Boss->GetControllerBase()->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, Player);
			}
		}
		
		UpdatedEntrance(true);

		Cast<IGameStateInterface>(GetWorld()->GetGameState())->StartBossArea(Player);
		Cast<IGameStateInterface>(GetWorld()->GetGameState())->SetCurrentBM(this);

		//if(StartPoint) StartPoint->StartSequence();
	}
}
