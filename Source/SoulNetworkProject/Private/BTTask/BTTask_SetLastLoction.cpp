// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_SetLastLoction.h"
#include "Classes/AIControllerBase.h"

#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetLastLoction::UBTTask_SetLastLoction(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = L"Set Last Location";
}

EBTNodeResult::Type UBTTask_SetLastLoction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetOwner());
	if(Con)
	{
		AActor* Target = Cast<AActor>(Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor));
		if(!Target) return EBTNodeResult::Failed;

		Con->GetBlackboard()->SetValueAsVector(BlackboardKey.SelectedKeyName, Target->GetActorLocation());
		return EBTNodeResult::Succeeded;
	}


	return EBTNodeResult::Failed;
}
