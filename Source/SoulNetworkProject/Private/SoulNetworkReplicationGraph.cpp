// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulNetworkReplicationGraph.h"
#include "Classes/PlayerControllerBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/SmartObject/WorldPickupItem.h"
#include "Classes/PickupActor.h"

#if WITH_GAMEPLAY_DEBUGGER
	#include "GameplayDebuggerCategoryReplicator.h"
#endif

#include "GameFramework/PlayerState.h"
#include "Engine/LevelScriptActor.h"
#include "ReplicationGraph.h"
#include "GameFramework/Info.h"
#include "SoulNetworkLog.h"

DEFINE_LOG_CATEGORY( LogSoulReplicationGraph );

int32 CVar_ShooterRepGraph_DisplayClientLevelStreaming = 0;
static FAutoConsoleVariableRef CVarShooterRepGraphDisplayClientLevelStreaming(TEXT("ShooterRepGraph.DisplayClientLevelStreaming"), CVar_ShooterRepGraph_DisplayClientLevelStreaming, TEXT(""), ECVF_Default);

USoulNetworkReplicationGraph::USoulNetworkReplicationGraph()
{

}

void USoulNetworkReplicationGraph::ResetGameWorldState()
{
	Super::ResetGameWorldState();
	AlwaysRelevantStreamingLevelActors.Empty();

	for (auto& ConnectionList : { Connections, PendingConnections })
	{
		for (UNetReplicationGraphConnection* ConnManager : ConnectionList)
		{
			for (UReplicationGraphNode* ConnectionNode : ConnManager->GetConnectionGraphNodes())
			{
				if (USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection* AlwaysRelevantConnectionNode = Cast<USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection>(ConnectionNode))
				{
					AlwaysRelevantConnectionNode->ResetGameWorldState();
				}
			}
		}

		for (UNetReplicationGraphConnection* ConnManager : PendingConnections)
		{
			for (UReplicationGraphNode* ConnectionNode : ConnManager->GetConnectionGraphNodes())
			{
				if (USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection* AlwaysRelevantConnectionNode = Cast<USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection>(ConnManager))
				{
					AlwaysRelevantConnectionNode->ResetGameWorldState();
				}
			}
		}
	}
}

void InitClassReplicationInfo(FClassReplicationInfo& Info, UClass* Class, bool bSpatialize, float ServerMaxTickRate)
{
	AActor* CDO = Class->GetDefaultObject<AActor>();
	if (bSpatialize)
	{
		Info.SetCullDistanceSquared(CDO->NetCullDistanceSquared);
		UE_LOG(LogSoulNetwork, Warning, TEXT("Setting cull distance for %s to %f (%f)"), *Class->GetName(), Info.GetCullDistanceSquared(), Info.GetCullDistance());
	}

	Info.ReplicationPeriodFrame = FMath::Max<uint32>( (uint32)FMath::RoundToFloat(ServerMaxTickRate / CDO->NetUpdateFrequency), 1);

	UClass* NativeClass = Class;
	while (!NativeClass->IsNative() && NativeClass->GetSuperClass() && NativeClass->GetSuperClass() != AActor::StaticClass())
	{
		NativeClass = NativeClass->GetSuperClass();
	}

	UE_LOG(LogSoulNetwork, Log, TEXT("Setting replication period for %s (%s) to %d frames (%.2f)"), *Class->GetName(), *NativeClass->GetName(), Info.ReplicationPeriodFrame, CDO->NetUpdateFrequency);
}

void USoulNetworkReplicationGraph::InitGlobalActorClassSettings()
{
	Super::InitGlobalActorClassSettings();

	/*
	* 프로그래밍 방식으로 규칙을 작성합니다.
	*/
	auto AddInfo = [&]( UClass* Class, EClassRepNodeMapping Mapping) { ClassRepNodePolicies.Set(Class, Mapping); };

	AddInfo(AWorldPickupItem::StaticClass(),			EClassRepNodeMapping::NotRouted);		// 종속 행위자 복제(Pawn)를 통해 처리됩니다.
	AddInfo(APlayerState::StaticClass(),				EClassRepNodeMapping::NotRouted);
	AddInfo(ALevelScriptActor::StaticClass(),			EClassRepNodeMapping::NotRouted);
	AddInfo(AReplicationGraphDebugActor::StaticClass(), EClassRepNodeMapping::NotRouted);				// Not needed. Replicated special case inside RepGraph
	AddInfo(AInfo::StaticClass(),						EClassRepNodeMapping::RelevantAllConnections);	// Non spatialized, relevant to all
	AddInfo(ASmartObject::StaticClass(),				EClassRepNodeMapping::Spatialize_Static);
	//AddInfo(APlayerCharacter::StaticClass(),			EClassRepNodeMapping::Spatialize_Dynamic);

//#if WITH_GAMEPLAY_DEBUGGER
//	AddInfo(AGameplayDebuggerCategoryReplicator::StaticClass(), EClassRepNodeMapping::NotRouted);				// Replicated via UShooterReplicationGraphNode_AlwaysRelevant_ForConnection
//#endif

	TArray<UClass*> AllReplicatedClasses;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		AActor* ActorCDO = Cast<AActor>(Class->GetDefaultObject());
		if (!ActorCDO || !ActorCDO->GetIsReplicated())
		{
			continue;
		}

		// SKEL 또는 REINST 클래스는 넘깁니다. //
		if (Class->GetName().StartsWith(TEXT("SKEL_")) || Class->GetName().StartsWith(TEXT("REINST_")))
		{
			continue;
		}

		// ------------------------------------------------ -------
		// 복제된 클래스입니다. 아래의 두 번째 패스를 위해 이것을 저장하세요.
		// ------------------------------------------------ -------
		// ------------------------------------------------ -------
		AllReplicatedClasses.Add(Class);

		// 맵에 이미 있는 경우 건너뜁니다(명시적으로 추가됨). //
		if (ClassRepNodePolicies.Contains(Class, false))
		{
			continue;
		}

		auto ShouldSpatialize = [](const AActor* CDO)
		{
			return CDO->GetIsReplicated() && (!(CDO->bAlwaysRelevant || CDO->bOnlyRelevantToOwner || CDO->bNetUseOwnerRelevancy));
		};

		auto GetLegacyDebugStr = [](const AActor* CDO)
		{
			return FString::Printf(TEXT("%s [%d%d%d]"), *CDO->GetClass()->GetName(), CDO->bAlwaysRelevant, CDO->bOnlyRelevantToOwner, CDO->bNetUseOwnerRelevancy);
		};

		// ------------------------------------------------ ------------------------------------- ------------------------------------- ------------------
		// FClassReplicationInfo를 설정합니다. 이는 기본적으로 클래스별 복제 설정입니다. 일부는 명시적으로 설정하고 나머지는 AActor의 레거시 설정을 확인하여 설정합니다.
		// ------------------------------------------------ ------------------------------------- ------------------------------------- ------------------
		// ------------------------------------------------ ------------------------------------- ------------------------------------- ------------------

		TArray<UClass*> ExplicitySetClasses;
		auto SetClassInfo = [&](UClass* Class, const FClassReplicationInfo& Info) { GlobalActorReplicationInfoMap.SetClassInfo(Class, Info); ExplicitySetClasses.Add(Class); };

		FClassReplicationInfo PawnClassRepInfo;
		PawnClassRepInfo.DistancePriorityScale = 1.0f;
		PawnClassRepInfo.StarvationPriorityScale = 1.0f;
		PawnClassRepInfo.ActorChannelFrameTimeout = 4;
		PawnClassRepInfo.SetCullDistanceSquared(30000.0f * 30000.0f);
		SetClassInfo(APawn::StaticClass(), PawnClassRepInfo);

		FClassReplicationInfo PlayerStateRepInfo;
		PlayerStateRepInfo.DistancePriorityScale = 0.0f;
		PlayerStateRepInfo.ActorChannelFrameTimeout = 0;
		SetClassInfo( APlayerState::StaticClass(), PlayerStateRepInfo);

		UReplicationGraphNode_ActorListFrequencyBuckets::DefaultSettings.ListSize = 12;

		// 모든 Replicated 클래스들을 FClassReplicationInfo에 지정해놓는다. //
		/** 복제된 클래스가 노출되지 않도록하는 작업 **/
		for (UClass* ReplicatedClass : AllReplicatedClasses)
		{
			if (ExplicitySetClasses.FindByPredicate([&](const UClass* SetClass) { return ReplicatedClass->IsChildOf(SetClass); }) != nullptr)
			{
				continue;
			}

			//const bool bClassIsSpatialized = IsSpatialized(ClassRepNodePolicies.GetChecked(ReplicatedClass));	//수정//

			FClassReplicationInfo ClassInfo;
			InitClassReplicationInfo(ClassInfo, ReplicatedClass, true, NetDriver->NetServerMaxTickRate);
			GlobalActorReplicationInfoMap.SetClassInfo(ReplicatedClass, ClassInfo);
		}
	}
}

void USoulNetworkReplicationGraph::InitGlobalGraphNodes()
{
	// 공유할 목록을 미리 할당하는 단계. //
	PreAllocateRepList(3, 12);
	PreAllocateRepList(6, 12);
	PreAllocateRepList(128, 64);
	PreAllocateRepList(512, 16);

	// 그리드 생성. //
	GridNode = CreateNewNode<UReplicationGraphNode_GridSpatialization2D>();
	GridNode->CellSize = GridCellSize;
	GridNode->SpatialBias = FVector2D(SpatialBiasX, SpatialBiasY);
	
	if (bDisableSpatialRebuilding)
	{
		// 공간을 재구축을 안 할경우 우리가 사용중인 Actor 클래스들만 제외 후 정지 비활성화한다.
		GridNode->AddSpatialRebuildBlacklistClass(AActor::StaticClass());
	}

	AddGlobalGraphNode(GridNode);

	// 이미 해당된 노드들을 생성한다. //
	AlwaysRelevantNode = CreateNewNode<UReplicationGraphNode_ActorList>();
	AddGlobalGraphNode(AlwaysRelevantNode);
}

void USoulNetworkReplicationGraph::InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager)
{
	Super::InitConnectionGraphNodes(ConnectionManager);

	USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection* AlwaysRelevantConnectionNode = CreateNewNode<USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection>();

	// 이 노드는 클라이언트 레벨이 가시성에 들어오고 나가는 시기를 알아야 합니다. //
	ConnectionManager->OnClientVisibleLevelNameAdd.AddUObject(AlwaysRelevantConnectionNode, &USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityAdd);
	ConnectionManager->OnClientVisibleLevelNameRemove.AddUObject(AlwaysRelevantConnectionNode, &USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityRemove);

	AddConnectionGraphNode(AlwaysRelevantConnectionNode, ConnectionManager);
}

void USoulNetworkReplicationGraph::RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo)
{
	EClassRepNodeMapping MappingPolicy = GetMappingPolicy(ActorInfo.Class);
	switch (MappingPolicy)
	{
		case EClassRepNodeMapping::NotRouted:
		{
			break;
		}

		case EClassRepNodeMapping::RelevantAllConnections:
		{
			if (ActorInfo.StreamingLevelName == NAME_None)
			{
				AlwaysRelevantNode->NotifyAddNetworkActor(ActorInfo);
			}
			else
			{
				FActorRepListRefView& RepList = AlwaysRelevantStreamingLevelActors.FindOrAdd(ActorInfo.StreamingLevelName);
				RepList.PrepareForWrite();
				RepList.ConditionalAdd(ActorInfo.Actor);
			}
			break;
		}

		case EClassRepNodeMapping::Spatialize_Static:
		{
			GridNode->AddActor_Static(ActorInfo, GlobalInfo);
			break;
		}
		case EClassRepNodeMapping::Spatialize_Dynamic:
		{
			GridNode->AddActor_Dynamic(ActorInfo, GlobalInfo);

			break;
		}
		case EClassRepNodeMapping::Spatilaize_Dormancy:
		{
			GridNode->AddActor_Dormancy(ActorInfo, GlobalInfo);

			break;
		}
		default:
			break;
	}
}

void USoulNetworkReplicationGraph::RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo)
{
	EClassRepNodeMapping MappingPolicy = GetMappingPolicy(ActorInfo.Class);
	switch (MappingPolicy)
	{
	case EClassRepNodeMapping::NotRouted:
	{
		break;
	}

	case EClassRepNodeMapping::RelevantAllConnections:
	{
		if (ActorInfo.StreamingLevelName == NAME_None)
		{
			AlwaysRelevantNode->NotifyRemoveNetworkActor(ActorInfo);
		}
		else
		{
			FActorRepListRefView& RepList = AlwaysRelevantStreamingLevelActors.FindOrAdd(ActorInfo.StreamingLevelName);
			if (RepList.RemoveFast(ActorInfo.Actor) == false)
			{
				UE_LOG(LogSoulNetwork, Warning, TEXT("Actor %s was not found in AlwaysRelevantStreamingLevelActors list. LevelName: %s"), *GetActorRepListTypeDebugString(ActorInfo.Actor), *ActorInfo.StreamingLevelName.ToString());
			}
		}
		break;
	}

	case EClassRepNodeMapping::Spatialize_Static:
	{
		GridNode->RemoveActor_Static(ActorInfo);
		break;
	}
	case EClassRepNodeMapping::Spatialize_Dynamic:
	{
		GridNode->RemoveActor_Dynamic(ActorInfo);

		break;
	}
	case EClassRepNodeMapping::Spatilaize_Dormancy:
	{
		GridNode->RemoveActor_Dormancy(ActorInfo);

		break;
	}
	default:
		break;
	}
}

void USoulNetworkReplicationGraph::OnGameplayDebuggerOwnerChange(AGameplayDebuggerCategoryReplicator* Debugger, APlayerController* OldOwner)
{
}

EClassRepNodeMapping USoulNetworkReplicationGraph::GetMappingPolicy(UClass* InClass)
{
	EClassRepNodeMapping* PolicyPtr = ClassRepNodePolicies.Get(InClass);
	EClassRepNodeMapping Policy = PolicyPtr ? *PolicyPtr : EClassRepNodeMapping::NotRouted;

	return Policy;
}
/**/
void USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection::GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params)
{
	Super::GatherActorListsForConnection(Params);

	USoulNetworkReplicationGraph* SoulGraph = CastChecked<USoulNetworkReplicationGraph>(GetOuter());

	ReplicationActorList.Reset();

	auto ResetActorCullDistance = [&](AActor* ActorToSet, AActor*& LastActor)
	{
		if (ActorToSet != LastActor)
		{
			LastActor = ActorToSet;

			UE_LOG(LogSoulReplicationGraph, Verbose, TEXT("Setting Pawn cull Distance to 0. %s"), *ActorToSet->GetName());
			FConnectionReplicationActorInfo& ConnectionActorInfo = Params.ConnectionManager.ActorInfoMap.FindOrAdd(ActorToSet);
			ConnectionActorInfo.SetCullDistanceSquared(0.0f);
		}
	};

	for (const FNetViewer& CurViewer : Params.Viewers)
	{
		ReplicationActorList.ConditionalAdd(CurViewer.InViewer);
		ReplicationActorList.ConditionalAdd(CurViewer.ViewTarget);

		if (APlayerControllerBase* PC = Cast<APlayerControllerBase>(CurViewer.InViewer))
		{
			// PlayerStates를 50% 제한합니다.
			const bool bReplicatePS = (Params.ConnectionManager.ConnectionOrderNum % 2) == (Params.ReplicationFrameNum % 2);
			if (bReplicatePS)
			{
				// 항상 플레이어 상태를 소유 플레이어에게 반환합니다. 시뮬레이션된 프록시 플레이어 상태는 UShooterReplicationGraphNode_PlayerStateFrequencyLimiter에 의해 처리됩니다.
				if (APlayerState* PS = PC->PlayerState)
				{
					if (!bInitializedPlayerState)
					{
						bInitializedPlayerState = true;
						FConnectionReplicationActorInfo& ConnectionActorInfo = Params.ConnectionManager.ActorInfoMap.FindOrAdd(PS);
						ConnectionActorInfo.ReplicationPeriodFrame = 1;
					}
					ReplicationActorList.ConditionalAdd(PS);
				}

				FAlwaysRelevantActorInfo* LastData = PastRelevantActors.FindByKey<UNetConnection*>(CurViewer.Connection);

				// 이전에 이 액터를 본 적이 없으므로 추가해 보세요.
				if (LastData == nullptr)
				{
					FAlwaysRelevantActorInfo NewActorInfo;
					NewActorInfo.Connection = CurViewer.Connection;
					LastData = &(PastRelevantActors[PastRelevantActors.Add(NewActorInfo)]);
				}

				check(LastData != nullptr);

				if (APlayerCharacter* Pawn = Cast<APlayerCharacter>(PC->GetPawn()))
				{
					ResetActorCullDistance(Pawn, static_cast<AActor*&>(LastData->LastViewer));

					if (Pawn != CurViewer.ViewTarget)
					{
						ReplicationActorList.ConditionalAdd(Pawn);
					}

				}

				if (APlayerCharacter* ViewTargetPawn = Cast<APlayerCharacter>(CurViewer.ViewTarget))
				{
					ResetActorCullDistance(ViewTargetPawn, static_cast<AActor*&>(LastData->LastViewTarget));
				}
			}
		}
	}

	PastRelevantActors.RemoveAll([&](FAlwaysRelevantActorInfo& RelActorInfo)
	{
		return RelActorInfo.Connection == nullptr;
	});

	Params.OutGatheredReplicationLists.AddReplicationActorList(ReplicationActorList);

	// 항상 관련된 스트리밍 레벨 액터.
	FPerConnectionActorInfoMap& ConnectionActorInfoMap = Params.ConnectionManager.ActorInfoMap;

	TMap<FName, FActorRepListRefView>& AlwaysRelevantStreamingLevelActors = SoulGraph->AlwaysRelevantStreamingLevelActors;

	for (int32 index = AlwaysRelevantStreamingLevelActors.Num() - 1; index >= 0; --index)
	{
		const FName& StreamingLevel = AlwaysRelevantStreamingLevelsNeedingReplication[index];

		FActorRepListRefView* Ptr = AlwaysRelevantStreamingLevelActors.Find(StreamingLevel);
		if (Ptr == nullptr)
		{
			// 해당 레벨에 대해 항상 관련 있는 목록은 없습니다.
			//UE_LOG(CVar_ShooterRepGraph_DisplayClientLevelStreaming > 0, LogSoulReplicationGraph, Display, TEXT("CLIENTSTREAMING Removing %s from AlwaysRelevantStreamingLevelActors because FActorRepListRefView is null. %s "), *StreamingLevel.ToString(), *Params.ConnectionManager.GetName());
			AlwaysRelevantStreamingLevelsNeedingReplication.RemoveAtSwap(index, 1, false);
			continue;
		}

		FActorRepListRefView& RepList = *Ptr;
		if (RepList.Num() > 0)
		{
			bool bAllDormat = true;
			for (FActorRepListType Actor : RepList)
			{
				FConnectionReplicationActorInfo& ConnectionActorInfo = ConnectionActorInfoMap.FindOrAdd(Actor);
				if (ConnectionActorInfo.bDormantOnConnection == false)
				{
					bAllDormat = false;
					break;
				}

				if (bAllDormat)
				{
					UE_CLOG(CVar_ShooterRepGraph_DisplayClientLevelStreaming > 0, LogSoulReplicationGraph, Display, TEXT("CLIENTSTREAMING All AlwaysRelevant Actors Dormant on StreamingLevel %s for %s. Removing list."), * StreamingLevel.ToString(), * Params.ConnectionManager.GetName());
					AlwaysRelevantStreamingLevelsNeedingReplication.RemoveAtSwap(index, 1, false);
				}
				else
				{
					UE_CLOG(CVar_ShooterRepGraph_DisplayClientLevelStreaming > 0, LogSoulReplicationGraph, Display, TEXT("CLIENTSTREAMING Adding always Actors on StreamingLevel %s for %s because it has at least one non dormant actor"), *StreamingLevel.ToString(), *Params.ConnectionManager.GetName());
					Params.OutGatheredReplicationLists.AddReplicationActorList(RepList);
				}
			}
		}
		else
		{
			UE_LOG(LogSoulReplicationGraph, Warning, TEXT("UShooterReplicationGraphNode_AlwaysRelevant_ForConnection::GatherActorListsForConnection - empty RepList %s"), *Params.ConnectionManager.GetName());

		}
	}

#if WITH_GAMEPLAY_DEBUGGER
	if (GameplayDebugger)
	{
		ReplicationActorList.ConditionalAdd(GameplayDebugger);
	}
#endif
}

void USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityAdd(FName LevelName, UWorld* StreamingWorld)
{
	UE_CLOG(CVar_ShooterRepGraph_DisplayClientLevelStreaming > 0, LogSoulReplicationGraph, Display, TEXT("CLIENTSTREAMING ::OnClientLevelVisibilityAdd - %s"), *LevelName.ToString());
	AlwaysRelevantStreamingLevelsNeedingReplication.Add(LevelName);
}
void USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityRemove(FName LevelName)
{
	UE_CLOG(CVar_ShooterRepGraph_DisplayClientLevelStreaming > 0, LogSoulReplicationGraph, Display, TEXT("CLIENTSTREAMING ::OnClientLevelVisibilityRemove - %s"), *LevelName.ToString());
	AlwaysRelevantStreamingLevelsNeedingReplication.Remove(LevelName);
}

void USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection::ResetGameWorldState()
{
	AlwaysRelevantStreamingLevelsNeedingReplication.Empty();
}
