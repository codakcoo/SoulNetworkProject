// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_AIStateOverride.h"
#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AIStateOverride::UBTTask_AIStateOverride(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "AIState Override";
}

EBTNodeResult::Type UBTTask_AIStateOverride::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if(!Con) return EBTNodeResult::Failed;

	Con->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)DesiredState);

	return EBTNodeResult::Succeeded;
}
