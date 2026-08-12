// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "ImpactDamageType.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UImpactDamageType : public UDamageType
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	float GetTotalDamageValue(class ACharacterBase* InCauser, class ACharacterBase* InApply, float BaseDamageValue, EEquipHand HandType);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Delay")
	float DamagePossibleDelay = 0.5f;
};
