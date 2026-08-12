// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PlayEffectNotify.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UPlayEffectNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "PlaySocket")
	FName SocketName = NAME_None;
	UPROPERTY(EditAnywhere, Category = "Value")
	float EffectScale = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Value")
	float SoundVolume = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Asset")
	class UNiagaraSystem* PaticleAsset = nullptr;
	UPROPERTY(EditAnywhere, Category = "Asset")
	class USoundBase* SoundAsset = nullptr;
};
