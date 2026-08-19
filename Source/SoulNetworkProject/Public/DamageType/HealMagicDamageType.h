// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageType/MagicDamageType.h"
#include "Interface/DamageInterface.h"
#include "HealMagicDamageType.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UHealMagicDamageType : public UMagicDamageType, public IDamageInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void ApplyDamageEffect(class AActor* InCauser, class AActor* InApply, FVector InDirection, float DamageValue) override;

};
