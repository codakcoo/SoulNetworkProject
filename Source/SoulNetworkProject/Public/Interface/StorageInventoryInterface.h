// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StorageInventoryInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStorageInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API IStorageInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void UpdatedItemSlot(class USlotWidget* ItemSlot) = 0;
	UFUNCTION()
	virtual void UpdateItemText(class USlotWidget* ItemSlot) = 0;
};
