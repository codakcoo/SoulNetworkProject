// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "StateIncreaseNotify.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EIncreaseType : uint8
{
	Expendable		UMETA(DisplayName = "Expendable"),
	// Only Stamina. //
	Value		UMETA(DisplayName = "Value")
};

UCLASS()
class SOULNETWORKPROJECT_API UStateIncreaseNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "Value")
	EIncreaseType IncreaseType;
	UPROPERTY(EditAnywhere, Category = "Value", meta = (EditCondition = "IncreaseType == EIncreaseType::Value"))
	float IncreaseValue = 0.0f;
};
