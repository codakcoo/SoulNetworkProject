// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_EnemyInRange.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTTask_EnemyInRange : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_EnemyInRange(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	bool SphereTrace();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bDodge = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float Radius = 100.f;

private:
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceCannels;

	class ACharacterBase* Chr;
};
