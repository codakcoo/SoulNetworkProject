// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorator/BTDecorator_DistanceCheck.h"
#include "Classes/AIControllerBase.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_DistanceCheck::UBTDecorator_DistanceCheck(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Distance_Check";

	BlackboardKey.AddObjectFilter(this, *NodeName, AActor::StaticClass());
}

bool UBTDecorator_DistanceCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	bool OutsideRange = false;

	if(!Con) return OutsideRange;

	AActor* TargetActor = Cast<AActor>(Con->GetBlackboard()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if(!TargetActor) return OutsideRange;

	if(Con->GetPawn()->GetDistanceTo(TargetActor) >= Distance) OutsideRange = true;

	return OutsideRange;
}
