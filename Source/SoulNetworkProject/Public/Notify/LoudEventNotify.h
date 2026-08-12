// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "LoudEventNotify.generated.h"

class USoundCue;
/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ULoudEventNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Value")
	USoundBase* SoundAsset = nullptr;
};
