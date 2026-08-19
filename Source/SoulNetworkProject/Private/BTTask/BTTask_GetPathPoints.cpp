// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_GetPathPoints.h"
#include "Classes/AIControllerBase.h"
#include "Classes/SmartObject/PatrolPath.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetPathPoints::UBTTask_GetPathPoints(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Get Path Points";
}

EBTNodeResult::Type UBTTask_GetPathPoints::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if(!Con || !Con->GetBlackboard()) return EBTNodeResult::Failed;

	ACharacterBase* Chr = Cast<ACharacterBase>(Con->GetPawn());
	if(!Chr) return EBTNodeResult::Failed;

	APatrolPath* Path = Cast<APatrolPath>(Chr->SmartObject);
	if(!Path || Path->Locations.Num() < 1) return EBTNodeResult::Failed;

	Con->GetBlackboard()->SetValueAsVector(bb_keys::target_location, Path->Locations[Index]);

	if (Index < Path->Locations.Num() - 1)
	{
		Index++;
		return EBTNodeResult::Succeeded;
	}

	Index = 0;
	return EBTNodeResult::Succeeded;
}
