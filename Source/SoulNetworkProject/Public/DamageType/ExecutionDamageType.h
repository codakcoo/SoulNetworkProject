// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageType/ImpactDamageType.h"
#include "Interface/DamageInterface.h"
#include "ExecutionDamageType.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UExecutionDamageType : public UImpactDamageType, public IDamageInterface
{
	GENERATED_BODY()

public:
	//Interface//
	UFUNCTION()
	virtual void ApplyDamageEffect(class AActor* InCauser, class AActor* InApply, FVector InDirection, float DamageValue) override;
	////////////
};
