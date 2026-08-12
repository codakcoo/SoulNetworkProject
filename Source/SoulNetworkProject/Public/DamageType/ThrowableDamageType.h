// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "Interface/DamageInterface.h"
#include "ThrowableDamageType.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UThrowableDamageType : public UDamageType, public IDamageInterface
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	virtual void ApplyDamageEffect(class AActor* InCauser, class AActor* InApply, FVector InDirection, float DamageValue) override;

	void BasicDamageEvent(class ACharacterBase* InCauser, class ACharacterBase* InApply, FVector InDirection, float DamageValue);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Delay")
	float DamagePossibleDelay = 0.5f;
};
