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
	NotRouted,					// � ��忡�� ���ε��� �ʽ��ϴ�. Ư�� ���̽� ��忡�� ó���ϴ� Ư�� ���̽� ���Ϳ� ���˴ϴ�.
	RelevantAllConnections,		// AlwaysRelevantNode �Ǵ� AlwaysRelevantStreamingLevelNode ���� ������մϴ�.

	// �׸��� ������ ����ȭ�� ���.

	Spatialize_Static,			// GridNode�� �����: �� ���ʹ� �������� ������ �� ������ ������Ʈ�� �ʿ䰡 �����ϴ�.
	Spatialize_Dynamic,			// GridNode�� �����: �� ���� ���� ���� �̷������ �����Ӵ� �� ���� ������Ʈ�˴ϴ�.
	Spatilaize_Dormancy			// GridNode�� �����: �޸� ������ ���ȿ��� �������� ó���˴ϴ�. �÷���/�޸� ������ �ƴ� ���. �̴� "�޸� ���°� �ƴ� ���� �̵�"�ϴ� �׸� ���� ���Դϴ�.
};

/** SoulNetwork Replication Graph implementation. **/
UCLASS(transient, config = Engine)
class SOULNETWORKPROJECT_API USoulNetworkReplicationGraph : public UReplicationGraph
{
	GENERATED_BODY()

	USoulNetworkReplicationGraph();
	
	virtual void ResetGameWorldState() override;

	// UReplicationGraph ���� �Լ�. //
	virtual void InitGlobalActorClassSettings() override;
	virtual void InitGlobalGraphNodes() override;
	virtual void InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager) override;
	// rout ���� �Լ�. //
	virtual void RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo) override;
	virtual void RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo) override;
	// UReplicationGraph �� �Լ�. //
	//virtual void ;

public:
#if WITH_GAMEPLAY_DEBUGGER
	void OnGameplayDebuggerOwnerChange(AGameplayDebuggerCategoryReplicator* Debugger, APlayerController* OldOwner);
#endif

protected:
	// ������ Ŭ������ ����� ������ �����ɴϴ�. //
	EClassRepNodeMapping GetMappingPolicy(UClass* InClass);

	FORCEINLINE bool IsSpatialized(EClassRepNodeMapping Mapping) { return Mapping >= EClassRepNodeMapping::Spatialize_Static; }
public:
	/*
	* �ش� �������� ���ø����̼� �׷������� �߿��� �����̴�.
	* ������ �׸���� �����ϰ� �����ڰ� ���� ���ǵ� �پ��� 
	* �׸����� ���ῡ ��Ʈ��ũ ������Ʈ�� ������ �ϴ��� �����մϴ�.
	*/
	UPROPERTY()
	UReplicationGraphNode_GridSpatialization2D* GridNode;

	UPROPERTY()
	UReplicationGraphNode_ActorList* AlwaysRelevantNode;

	// ��Ʈ���ָ��� �̹� �����ִ� ���͵�. //
	TMap<FName, FActorRepListRefView> AlwaysRelevantStreamingLevelActors;

protected:

	UPROPERTY()
	TArray<UClass*> SpatializedClasses;
	UPROPERTY()
	TArray<UClass*> NonSpatializedClasses;
	UPROPERTY()
	TArray<UClass*> AlwaysRelevantClasses;

	/*
	* ���� �籸�� = 
	* ����ȭ Ʈ���� ������ �籸���մϴ�. 
	* ��Ȱ��ȭ�ϸ� ��� �����ȴٴ� �ǹ��Դϴ�.
	*/
	float GridCellSize = 10000.0f;				// �׸��� �ϳ��� ������ ũ��.
	float SpatialBiasX = -150000.0f;			// ������ X ũ��. 
	float SpatialBiasY = -200000.0f;			// ������ Y ũ��.
	bool bDisableSpatialRebuilding = true;		// ������ �籸�������� ���� ����

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
	// ���� ��Ʈ���ֿ� ���ϴ� ���͵��� ����. //
	TArray<FName, TInlineAllocator<64>> AlwaysRelevantStreamingLevelsNeedingReplication;

	TArray<FAlwaysRelevantActorInfo> PastRelevantActors;

	FActorRepListRefView ReplicationActorList;

	bool bInitializedPlayerState = false;
};
