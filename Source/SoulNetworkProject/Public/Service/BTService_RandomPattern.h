// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_RandomPattern.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UBTService_RandomPattern : public UBTService_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTService_RandomPattern(const FObjectInitializer& ObjectInitializer);

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
