// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_CheckAroundEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTTask_CheckAroundEnemy : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	UBTTask_CheckAroundEnemy(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float Search_Radius = 500.f;

private:
	TArray<AActor*> EnemyActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
};
