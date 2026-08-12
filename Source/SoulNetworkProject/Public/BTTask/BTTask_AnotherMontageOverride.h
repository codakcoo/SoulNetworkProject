// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SoulNetworkProject/ItemState.h"
#include "BTTask_AnotherMontageOverride.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class ERunMtType : uint8
 {
	 Custom		UMETA(DisplayName = "Custom"),
	 Equip		UMETA(DisplayName = "Equip"),
	 Dodge		UMETA(DisplayName = "Dodge")
 };

 UENUM(BlueprintType)
 enum class EDodgeType : uint8
 {
	Forward		UMETA(DisplayName = "Forward"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	Back		UMETA(DisplayName = "Back")
 };

UCLASS()
class SOULNETWORKPROJECT_API UBTTask_AnotherMontageOverride : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	UBTTask_AnotherMontageOverride(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	ERunMtType RunType = ERunMtType::Custom;
	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (EditCondition = "RunType == ERunMtType::Dodge"))
	EDodgeType DodgeType = EDodgeType::Forward;
	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (EditCondition = "RunType == ERunMtType::Equip"))
	EEquipHand EquipType = EEquipHand::Right;

	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (EditCondition = "RunType == ERunMtType::Custom"))
	class UAnimMontage* CustomMt = nullptr;
	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (EditCondition = "RunType == ERunMtType::Custom"))
	FName SectionName = NAME_None;
};
