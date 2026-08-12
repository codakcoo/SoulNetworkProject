// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_SwitchStrafeDirection.h"
#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/CharacterState.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SwitchStrafeDirection::UBTTask_SwitchStrafeDirection(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Switch Strafe";
}

EBTNodeResult::Type UBTTask_SwitchStrafeDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (!Con || !Con->GetBlackboard()) return EBTNodeResult::Failed;

	if (Con->GetBlackboard()->GetValueAsEnum(bb_keys::combat_state) == (uint8)ECombatState::Strafe)
	{
		Con->StrafeDirection *= -1.f;
	}
	return EBTNodeResult::Succeeded;
}
