// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "SoulNetworkProject/CharacterState.h"
#include "BTService_DistanceCheck.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTService_DistanceCheck : public UBTService_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTService_DistanceCheck(const FObjectInitializer& ObjectInitializer);

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	bool bInverse = false;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float Distance = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	EAIState DesiredAIState;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	ECombatState DesiredCombatState;
};
