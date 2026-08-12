// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_FindRandomLocation.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"

#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	ACharacterBase* Agent = Cast<ACharacterBase>(Con->GetPawn());
	if(!Con || !Agent) return EBTNodeResult::Failed;

	FVector Origin = Agent->GetActorLocation();
	FNavLocation FindLocation;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, SearchRadius, FindLocation, nullptr))
	{
		Con->GetBlackboard()->SetValueAsVector(GetSelectedBlackboardKey(), FindLocation.Location);
	}

	return EBTNodeResult::Succeeded;
}
