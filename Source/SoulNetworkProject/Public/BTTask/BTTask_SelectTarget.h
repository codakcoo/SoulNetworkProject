// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "BTTask_SelectTarget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTTask_SelectTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_SelectTarget(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void EnemySeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	UEnvQuery* EnemySeekerQuery;	

private:
	FEnvQueryRequest EnemySeekerQueryRequest;

	class AAIControllerBase* Con;

	class ACharacterBase* BestTarget;
};
