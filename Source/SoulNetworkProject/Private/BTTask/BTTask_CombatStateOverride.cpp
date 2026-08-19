// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_CombatStateOverride.h"
#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CombatStateOverride::UBTTask_CombatStateOverride(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Combat Override";
}

EBTNodeResult::Type UBTTask_CombatStateOverride::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if(!Con) return EBTNodeResult::Failed;

	Con->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)DesiredState);

	Con->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
	//if (DesiredState == ECombatState::Strafe)
	//{
	//	Con->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);
	//}

	return EBTNodeResult::Succeeded;
}
