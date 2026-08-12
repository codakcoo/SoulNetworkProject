// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_PlayMontage.h"

UBTTask_PlayMontage::UBTTask_PlayMontage(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Play Montage";
}

EBTNodeResult::Type UBTTask_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	

	return EBTNodeResult::Failed;
}
