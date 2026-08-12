// Fill out your copyright notice in the Description page of Project Settings.


#include "Service/BTService_RandomPattern.h"
#include "Classes/AIControllerBase.h"

#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_RandomPattern::UBTService_RandomPattern(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = L"Random Pattern";
}

void UBTService_RandomPattern::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if(!Con) return;

	int8 i = FMath::RandRange(1, 10);
	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, i);
}
