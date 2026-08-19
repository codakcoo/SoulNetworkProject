// Fill out your copyright notice in the Description page of Project Settings.


#include "Service/BTService_UpdatedPhase.h"
#include "Classes/AIControllerBase.h"
#include "Classes/BossCharacter.h"
#include "Classes/CharacterStateComponent.h"

#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdatedPhase::UBTService_UpdatedPhase(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Updated Phase";
}

void UBTService_UpdatedPhase::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	ABossCharacter* Chr = Cast<ABossCharacter>(Con->GetPawn());
	if(!Con || !Chr) return;

	Con->GetBlackboard()->SetValueAsInt(bb_keys::phase, Chr->GetBossPhaseIndex());
}