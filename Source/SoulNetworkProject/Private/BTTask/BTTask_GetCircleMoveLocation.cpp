// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_GetCircleMoveLocation.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"


UBTTask_GetCircleMoveLocation::UBTTask_GetCircleMoveLocation(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Get Circle MoveLocation";
}

EBTNodeResult::Type UBTTask_GetCircleMoveLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	Chr = Cast<ACharacterBase>(Con->GetPawn());

	if (Con && Chr)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(MoveTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(ValueChangeTimer, this, &UBTTask_GetCircleMoveLocation::ChangeValue, Delay + UKismetMathLibrary::RandomFloatInRange(0.f, RandomDeviation), true, 0.f);
			GetWorld()->GetTimerManager().SetTimer(MoveTimer, this, &UBTTask_GetCircleMoveLocation::UpdatedMoveLocation, 0.f, true, 0.f);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_GetCircleMoveLocation::UpdatedMoveLocation()
{
	Chr->MoveRight(DirectionValue);
}

void UBTTask_GetCircleMoveLocation::ChangeValue()
{
	if(DirectionValue >= 1.f) DirectionValue = -1.f;
	else if(DirectionValue <= -1.f) DirectionValue = 1.f;
}
