// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdatedPhase.generated.h"

/**
 * 
 */


UCLASS()
class SOULNETWORKPROJECT_API UBTService_UpdatedPhase : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	UBTService_UpdatedPhase(const FObjectInitializer& ObjectInitializer);

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
