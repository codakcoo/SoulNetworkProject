// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AllMeshFadeoutNotify.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class EFadeoutType : uint8
 {
	In		UMETA(DisplayName = "In"),
	Out		UMETA(DisplayName = "Out")
 };

UCLASS()
class SOULNETWORKPROJECT_API UAllMeshFadeoutNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "Value")
	EFadeoutType RunType = EFadeoutType::In;
};
