// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SoulNetworkProject/ItemState.h"
#include "Classes/CombatComponent.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void UpdatePointDamage(AActor* DamageCauser) = 0;
	UFUNCTION()
	virtual void UpdateRadialDamage(AActor* DamageCauser) = 0;
	UFUNCTION()
	virtual void UpdatedExecutionDamage(AActor* DamageCauser) = 0;
	UFUNCTION()
	virtual void UpdatedHitSurface(FHitResult HitResult, EAttackType AttackType) = 0;

	UFUNCTION()
	virtual void UpdatedSpell(float Value) = 0;
	UFUNCTION()
	virtual void UpdatedDeathFunc() = 0;

	UFUNCTION()
	virtual float GetApplyDamge(EEquipHand HandType) const = 0;
};
