// Fill out your copyright notice in the Description page of Project Settings.


#include "Service/BTService_UpdatedSight.h"
#include "Classes/AIControllerBase.h"

#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdatedSight::UBTService_UpdatedSight(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = L"Updated Sight";
}

void UBTService_UpdatedSight::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetOwner());
	if(Con->GetHasEnemysInSight()) CurrentTickTime = 0.0f;

	CurrentTickTime += DeltaSeconds;
	if (CurrentTickTime >= MaxTickTime)
	{
		Con->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)DesiredAIState);
		Con->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)DesiredCombatState);
		//Con->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
	}
}
