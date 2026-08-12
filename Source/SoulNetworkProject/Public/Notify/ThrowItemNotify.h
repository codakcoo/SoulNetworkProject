// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ThrowItemNotify.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EThrowType : uint8
{
	Expendable	UMETA(DisplayName = "Expendable"),
	Arrow		UMETA(DisplayName = "Arrow")
};

UCLASS()
class SOULNETWORKPROJECT_API UThrowItemNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	EThrowType ThrowType;
};
