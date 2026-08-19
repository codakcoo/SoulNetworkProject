// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "StopMovementNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UStopMovementNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
private:
	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

private:
	class APlayerCharacter* Owner;
};
