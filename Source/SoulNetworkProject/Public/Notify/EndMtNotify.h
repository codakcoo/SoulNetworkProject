// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EndMtNotify.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UEndMtNotify : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
