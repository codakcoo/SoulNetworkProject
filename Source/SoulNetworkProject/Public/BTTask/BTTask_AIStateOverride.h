// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SoulNetworkProject/CharacterState.h"
#include "BTTask_AIStateOverride.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ERunMode : uint8
{
	ThisAgent	UMETA(DisplayName = "ThisAgent"),
	AllAgents	UMETA(DisplayName = "AllAgents")
};

UCLASS()
class SOULNETWORKPROJECT_API UBTTask_AIStateOverride : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_AIStateOverride(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	EAIState DesiredState = EAIState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	ERunMode RunMode;
};
