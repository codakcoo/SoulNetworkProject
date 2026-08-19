// Fill out your copyright notice in the Description page of Project Settings.


#include "Service/BTService_DistanceCheck.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"

#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_DistanceCheck::UBTService_DistanceCheck(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Distance Check";
}

void UBTService_DistanceCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	AActor* TargetActor = Cast<AActor>(Con->GetBlackboard()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	bool bSee = Con->GetBlackboard()->GetValueAsBool(bb_keys::can_see_player);
	if (Con && TargetActor)
	{
		if(Con->GetBlackboard()->GetValueAsEnum(bb_keys::ai_status) != (uint8)EAIState::Attack && !bSee) return;

		if (!bInverse)
		{
			if (Con->GetPawn()->GetDistanceTo(TargetActor) < Distance)
			{
				Con->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)DesiredAIState);
				Con->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)DesiredCombatState);
				//Con->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
			}
		}
		else
		{
			if (Con->GetPawn()->GetDistanceTo(TargetActor) >= Distance)
			{
				Con->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)DesiredAIState);
				Con->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)DesiredCombatState);
				//Con->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
			}
		}
	}
}
