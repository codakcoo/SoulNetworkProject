// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AttackOverride.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class ERunAttackType : uint8
 {
	 LowAttack			UMETA(DisplayName = "LowAttack"),
	 HeavyAttack		UMETA(DisplayName = "HeavyAttack"),
	 Roll				UMETA(DisplayName = "Roll"),
	 AOEAttack			UMETA(DisplayName = "AOEAttack")
 };

UCLASS()
class SOULNETWORKPROJECT_API UBTTask_AttackOverride : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_AttackOverride(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool OverlapTarget();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	bool bUseSearchRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float Search_Radius = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	ERunAttackType	AttackType = ERunAttackType::LowAttack;


private:
	class ACharacterBase* Chr;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

};
