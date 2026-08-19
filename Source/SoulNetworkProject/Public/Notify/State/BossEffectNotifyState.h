// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BossEffectNotifyState.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class EEffectObjectRunType : uint8
 {
	Single		UMETA(DisplayName = "Single"),
	Multi		UMETA(DisplayName = "Multi")
 };

UCLASS()
class SOULNETWORKPROJECT_API UBossEffectNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Value")
	EEffectObjectRunType RunType = EEffectObjectRunType::Single;
};
