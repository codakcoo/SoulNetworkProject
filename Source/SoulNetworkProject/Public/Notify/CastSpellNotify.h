// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CastSpellNotify.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class ESpellCastType : uint8
 {
	Begin		UMETA(DisplayName = Begin),
	Loop		UMETA(DisplayName = Loop),
	End			UMETA(DisplayName = End)
 };

UCLASS()
class SOULNETWORKPROJECT_API UCastSpellNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "Type")
	ESpellCastType CastType;
};
