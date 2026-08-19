// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "SoulNetworkProject/BuffState.h"
#include "MagicDamageType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EStrongType : uint8
{
	Low		UMETA(DisplayName = "Low"),
	Heavy	UMETA(DisplayName = "Heavy")
};

UCLASS()
class SOULNETWORKPROJECT_API UMagicDamageType : public UDamageType
{
	GENERATED_BODY()
	

protected:
	UFUNCTION()
	float GetTotalDamageValue(class ACharacterBase* InCauser, class ACharacterBase* InApply, float BaseDamageValue);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	EBuffElementType SpellType = EBuffElementType::STR;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	EStrongType StrongType = EStrongType::Low;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float DamagePossibleDelay = 0.5f;
};
