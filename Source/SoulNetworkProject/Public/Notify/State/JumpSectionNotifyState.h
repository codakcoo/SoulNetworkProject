// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "JumpSectionNotifyState.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class EJumpCondition : uint8
 {
	Falling		UMETA(DisplayName = "Falling"),		// 떨어지는중에 
	Timer		UMETA(DisplayName = "Timer"),		// 타이머
	Trigger		UMETA(DisplayName = "Trigger")		// 트리거 (End에서 호출 됨.)
 };

UCLASS()
class SOULNETWORKPROJECT_API UJumpSectionNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "Condition")
	EJumpCondition Condition;
	UPROPERTY(EditAnywhere, Category = "Condition")
	FName JumpSection = NAME_None;
	UPROPERTY(EditAnywhere, Category = "Condition")//, EditCondition = "Condition == Timer")
	float JumpDelay = 0.0f;

private:
	FTimerHandle JumpTimer;
};
