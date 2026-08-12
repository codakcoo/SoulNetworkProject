// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "SoulNetworkProject/CharacterState.h"
#include "BTService_UpdatedSight.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTService_UpdatedSight : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	UBTService_UpdatedSight(const FObjectInitializer& ObjectInitializer);

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float MaxTickTime = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	EAIState DesiredAIState;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	ECombatState DesiredCombatState;

private:
	float CurrentTickTime = 0.0f;

};
