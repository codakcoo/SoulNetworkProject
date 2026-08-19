// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SoulNetworkProject/ItemData.h"
#include "Classes/CombatComponent.h"
#include "WeaponCollisionNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UWeaponCollisionNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "Value")
	EEquipHand HandType;

	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "HandType == EEquipHand::None"))
	FName StartSocket = NAME_None;
	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "HandType == EEquipHand::None"))
	FName EndSocket = NAME_None;
	UPROPERTY(EditAnywhere, Category = "Value")
	EAttackType PermuteType = EAttackType::None;
};
