// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReplicationGraph.h"
#include "SoulNetworkReplicationGraph.generated.h"


class UReplicationGraphNode_ActorList;
class UReplicationGraphNode_GridSpatialization2D;
class AGameplayDebuggerCategoryReplicator;

DECLARE_LOG_CATEGORY_EXTERN(LogSoulReplicationGraph, Display, All);

enum class EClassRepNodeMapping : uint8
{
	NotRouted,					// 어떤 노드에도 매핑되지 않습니다. 특수 케이스 노드에서 처리하는 특수 케이스 액터에 사용됩니다.
	RelevantAllConnections,		// AlwaysRelevantNode 또는 AlwaysRelevantStreamingLevelNode 노드로 라우팅합니다.

	// 그리드 노드로의 공간화된 경로.

	Spatialize_Static,			// GridNode로 라우팅: 이 액터는 움직이지 않으며 매 프레임 업데이트할 필요가 없습니다.
	Spatialize_Dynamic,			// GridNode로 라우팅: 이 액터 모드는 자주 이루어지며 프레임당 한 번씩 업데이트됩니다.
	Spatilaize_Dormancy			// GridNode로 라우팅: 휴면 상태인 동안에는 정적으로 처리됩니다. 플러시/휴면 동적이 아닌 경우. 이는 "휴면 상태가 아닌 동안 이동"하는 항목에 대한 것입니다.
};

/** SoulNetwork Replication Graph implementation. **/
UCLASS(transient, config = Engine)
class SOULNETWORKPROJECT_API USoulNetworkReplicationGraph : public UReplicationGraph
{
	GENERATED_BODY()

	USoulNetworkReplicationGraph();
	
	virtual void ResetGameWorldState() override;

	// UReplicationGraph 시작 함수. //
	virtual void InitGlobalActorClassSettings() override;
	virtual void InitGlobalGraphNodes() override;
	virtual void InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager) override;
	// rout 관련 함수. //
	virtual void RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo) override;
	virtual void RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo) override;
	// UReplicationGraph 끝 함수. //
	//virtual void ;

public:
#if WITH_GAMEPLAY_DEBUGGER
	void OnGameplayDebuggerOwnerChange(AGameplayDebuggerCategoryReplicator* Debugger, APlayerController* OldOwner);
#endif

protected:
	// 지정된 클래스에 사용할 매핑을 가져옵니다. //
	EClassRepNodeMapping GetMappingPolicy(UClass* InClass);

	FORCEINLINE bool IsSpatialized(EClassRepNodeMapping Mapping) { return Mapping >= EClassRepNodeMapping::Spatialize_Static; }
public:
	/*
	* 해당 변수들은 리플리케이션 그래프에서 중요한 노드들이다.
	* 지도를 그리드로 분할하고 행위자가 사전 정의된 다양한 
	* 그리드의 연결에 네트워크 업데이트를 보내야 하는지 결정합니다.
	*/
	UPROPERTY()
	UReplicationGraphNode_GridSpatialization2D* GridNode;

	UPROPERTY()
	UReplicationGraphNode_ActorList* AlwaysRelevantNode;

	// 스트리밍맵의 이미 관련있는 액터들. //
	TMap<FName, FActorRepListRefView> AlwaysRelevantStreamingLevelActors;

protected:

	UPROPERTY()
	TArray<UClass*> SpatializedClasses;
	UPROPERTY()
	TArray<UClass*> NonSpatializedClasses;
	UPROPERTY()
	TArray<UClass*> AlwaysRelevantClasses;

	/*
	* 공간 재구성 = 
	* 공간화 트리를 강제로 재구성합니다. 
	* 비활성화하면 대신 고정된다는 의미입니다.
	*/
	float GridCellSize = 10000.0f;				// 그리드 하나의 사이즈 크기.
	float SpatialBiasX = -150000.0f;			// 공유할 X 크기. 
	float SpatialBiasY = -200000.0f;			// 공유할 Y 크기.
	bool bDisableSpatialRebuilding = true;		// 공간을 재구성할지에 대한 여부

private:
	TClassMap<EClassRepNodeMapping> ClassRepNodePolicies;
};

UCLASS()
class USoulNetworkReplicationGraphNode_AlwaysRelevant_ForConnection : public UReplicationGraphNode
{
	GENERATED_BODY()

protected:
	virtual void NotifyAddNetworkActor(const FNewReplicatedActorInfo& Actor) override { }
	virtual bool NotifyRemoveNetworkActor(const FNewReplicatedActorInfo& ActorInfo, bool bWarnIfNotFound = true) override { return false; }
	virtual void NotifyResetAllNetworkActors() override { }

	virtual void GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params) override;
public:
	void OnClientLevelVisibilityAdd(FName LevelName, UWorld* StreamingWorld);
	void OnClientLevelVisibilityRemove(FName LevelName);


	void ResetGameWorldState();

public:
#if WITH_GAMEPLAY_DEBUGGER
	AGameplayDebuggerCategoryReplicator* GameplayDebugger = nullptr;
#endif

private:
	// 레벨 스트리밍에 속하는 액터들을 저장. //
	TArray<FName, TInlineAllocator<64>> AlwaysRelevantStreamingLevelsNeedingReplication;

	TArray<FAlwaysRelevantActorInfo> PastRelevantActors;

	FActorRepListRefView ReplicationActorList;

	bool bInitializedPlayerState = false;
};
