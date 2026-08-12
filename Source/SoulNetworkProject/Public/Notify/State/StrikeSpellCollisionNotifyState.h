// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "StrikeSpellCollisionNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UStrikeSpellCollisionNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
private:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
