// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "BTTask_FindStrafeLocation.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTTask_FindStrafeLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_FindStrafeLocation(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void LocationSeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	bool IsApplicableLocation(FVector Location);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	UEnvQuery* LocationSeekerQuery;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float ConditionRadius = 30.f;

	FVector StrafeLocation = FVector::ZeroVector;

private:
	class AAIControllerBase* Con;

	FEnvQueryRequest LocationSeekerQueryRequest;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
};
