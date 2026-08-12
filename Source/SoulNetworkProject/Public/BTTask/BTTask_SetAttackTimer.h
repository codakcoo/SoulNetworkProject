// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetAttackTimer.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTTask_SetAttackTimer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTTask_SetAttackTimer(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bActiveTimer = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float AttackTimerMin = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float AttacktimerMax = 5.0f;
};
