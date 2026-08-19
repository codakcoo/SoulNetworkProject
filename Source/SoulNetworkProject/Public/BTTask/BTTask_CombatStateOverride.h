// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SoulNetworkProject/CharacterState.h"
#include "BTTask_CombatStateOverride.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTTask_CombatStateOverride : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTTask_CombatStateOverride(const FObjectInitializer& ObjectInitializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blackboard")
	ECombatState DesiredState = ECombatState::Attack;
};
