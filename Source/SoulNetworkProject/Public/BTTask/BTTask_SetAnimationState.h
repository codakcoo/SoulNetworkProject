// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetAnimationState.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ESpeedType : uint8
{
	Walk	UMETA(DisplayName = "Walk"),
	Sprint	UMETA(DisplayName = "Sprint"),
	Slow	UMETA(DisplayName = "Slow")
};

UCLASS()
class SOULNETWORKPROJECT_API UBTTask_SetAnimationState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_SetAnimationState(const FObjectInitializer& ObjectInitialzier);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	float GetWalkSpeed(class ACharacterBase* Owner);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	ESpeedType Speed = ESpeedType::Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bADS = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bCombat = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bGuard = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bFocus = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bAim = false;

};
