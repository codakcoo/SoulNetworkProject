// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
//#include "Engine/EngineTypes.h"
#include "MotionWarpingNotifyState.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class EHomingType : uint8
 {
	 Target		UMETA(DisplayName = "Target"),
	 None		UMETA(DisplayName = "None")
 };

UCLASS()
class SOULNETWORKPROJECT_API UMotionWarpingNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "MotionWarping")
	EHomingType HomingType;

	UPROPERTY(EditAnywhere, Category = "MotionWarping")
	TEnumAsByte<enum EMovementMode> BeginMovementMode;
	UPROPERTY(EditAnywhere, Category = "MotionWarping")
	TEnumAsByte<enum EMovementMode> EndMovementMode;


	UPROPERTY(EditAnywhere, Category = "MotionWarping")
	float InterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "MotionWarping|WrapVector")//, meta = (EditCondition = "RangeType == ERangeType::Around"))
	FVector WrapVector = FVector::ZeroVector;
private:
	FVector TargetLocation = FVector::ZeroVector;
};
