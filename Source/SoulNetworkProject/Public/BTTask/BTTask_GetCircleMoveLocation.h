// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetCircleMoveLocation.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API UBTTask_GetCircleMoveLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
		
public:
	UBTTask_GetCircleMoveLocation(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void UpdatedMoveLocation();
	void ChangeValue();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float Delay = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float RandomDeviation = 0.f;

private:
	float DirectionValue = -1.f;
	FTimerHandle MoveTimer;
	FTimerHandle ValueChangeTimer;

	class ACharacterBase* Chr;
};
