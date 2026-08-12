// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetRandomCount.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTTask_SetRandomCount : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	UBTTask_SetRandomCount(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
