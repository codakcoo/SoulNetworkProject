// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_SetRandomCount.h"
#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "SoulNetworkLog.h"

UBTTask_SetRandomCount::UBTTask_SetRandomCount(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = L"Set Random Pattern";
}

EBTNodeResult::Type UBTTask_SetRandomCount::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if(!Con)
	{
		UE_LOG(LogSoulAI, Warning, TEXT("Not Found Con"));
		return EBTNodeResult::Failed;
	}

	int8 i  = FMath::RandRange(1, 10);

	//if (i <= 10)
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 1);
	//}
	//else if (i <= 20)
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 2);
	//
	//}
	//else if (i <= 30)
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 3);
	//
	//}
	//else if (i <= 40)
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 4);
	//
	//}
	//else if (i <= 50)
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 5);
	//
	//}
	//else if (i <= 60)
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 6);
	//
	//}
	//else if (i <= 70)
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 7);
	//
	//}
	//else if (i <= 80)
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 8);
	//
	//}
	//else if (i <= 90)
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 9);
	//}
	//else
	//{
	//	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, 10);
	//}

	//if(Con->GetBlackboard()->GetValueAsInt(bb_keys::random_count) < 0 || Con->GetBlackboard()->GetValueAsInt(bb_keys::random_count) > 10)
	//{
	//
	//	UE_LOG(LogTemp, Warning, TEXT("Over count"));
	//	return EBTNodeResult::Failed;
	//}
	Con->GetBlackboard()->SetValueAsInt(bb_keys::random_count, i);

	return EBTNodeResult::Succeeded;
}
