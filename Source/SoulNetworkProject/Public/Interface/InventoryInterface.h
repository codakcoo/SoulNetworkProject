// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API IInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void OnReaction(class USlotWidget* ItemSlot) = 0;

	UFUNCTION()
	virtual void OffReaction(class USlotWidget* ItemSlot) = 0;

	UFUNCTION()
	virtual void UpdatedWidgetText(class USlotWidget* ItemSlot) = 0;

	UFUNCTION()
	virtual void ActiveDropMenu() = 0;
	UFUNCTION()
	virtual void ActiveDestroyMenu() = 0;

	UFUNCTION()
	virtual void SetDropSelectMode() = 0;
	UFUNCTION()
	virtual void SetDestroySelectMode() = 0;

	UFUNCTION()
	virtual void UseReaction() = 0;

};
