// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SoulNetworkProject/ItemData.h"
#include "BTTask_CastSpellOverride.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API UBTTask_CastSpellOverride : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	UBTTask_CastSpellOverride(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
	bool OverlapTarget();

	UFUNCTION()
	void CancelChargeSpell();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bUseSearchRange = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	uint8 CastSpellCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float SearchRadius = 200.f;

private:
	class ACharacterBase* Chr;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	FInventoryData CurrentSpell;

	FTimerHandle ChargeTimer;
};
