// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_SelectTarget.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"


#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SelectTarget::UBTTask_SelectTarget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Select Target";
}

EBTNodeResult::Type UBTTask_SelectTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (Con && EnemySeekerQuery)
	{
		EnemySeekerQueryRequest = FEnvQueryRequest(EnemySeekerQuery, Con->Agent);
		EnemySeekerQueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &UBTTask_SelectTarget::EnemySeekerQueryFinished);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_SelectTarget::EnemySeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	BestTarget = nullptr;
	//Con->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, nullptr);

	float CurrentBestScore = 0.f;
	int32 Index = 0;
	TArray<AActor*> AllDetectedActors;
	Result->GetAllAsActors(AllDetectedActors);

	for (auto& DetectedActor : AllDetectedActors)
	{
		ACharacterBase* Chr = Cast<ACharacterBase>(DetectedActor);
		if (Con->Agent->GetFaction() == Chr->GetFaction())
		{
			Index++;

			continue;
		}


		if (Chr && Chr->IsHostile(Con->Agent))// !Chr->bDead)
		{
			if ((Result->GetItemScore(Index) > CurrentBestScore && Result->GetItemScore(Index) > 0.f) || (BestTarget == nullptr))
			{
				BestTarget = Chr;
				CurrentBestScore = Result->GetItemScore(Index);
			}
		}

		Index++;
	}

	if (BestTarget)
	{
		Con->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, BestTarget);
	}
}
