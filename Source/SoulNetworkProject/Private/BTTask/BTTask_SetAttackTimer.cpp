// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_SetAttackTimer.h"
#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetAttackTimer::UBTTask_SetAttackTimer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = L"Set Attack Timer";
}

EBTNodeResult::Type UBTTask_SetAttackTimer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIControllerBase* Con  = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (Con)
	{
		bActiveTimer ? Con->UpdatedAttackTimer(AttackTimerMin, AttacktimerMax) : Con->ClearAttackTimer();
	}

	return EBTNodeResult::Succeeded;
}
