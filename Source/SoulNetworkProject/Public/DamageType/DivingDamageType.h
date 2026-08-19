// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "Interface/DamageInterface.h"
#include "DivingDamageType.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UDivingDamageType : public UDamageType, public IDamageInterface
{
	GENERATED_BODY()
	
public:
	//Interface//
	UFUNCTION()
	virtual void ApplyDamageEffect(class AActor* InCauser, class AActor* InApply, FVector InDirection, float DamageValue) override;
	////////////
};
