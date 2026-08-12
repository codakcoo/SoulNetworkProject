// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpellEquipmentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USpellEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API ISpellEquipmentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//EquipSlotWidget Function//
	UFUNCTION()
	virtual void UpdateEquipWidgetText(class USpellType_SlotWidget* InSlot) = 0;
	UFUNCTION()
	virtual void OnSelectMenu() = 0;
	/////////////////////////////

	//EquipInvenSlotWidget Function//
	UFUNCTION()
	virtual void UpdateInvenWidgetText(class USpellType_InvenSlotWidget* InSlot) = 0;
	UFUNCTION()
	virtual void InOutItemSlot(class USlotWidget* ItemSlot) = 0;
	/////////////////////////////////
};
