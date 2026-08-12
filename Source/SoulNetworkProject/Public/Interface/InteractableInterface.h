// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void Interact(class APlayerCharacter* Character) = 0;

	UFUNCTION()
	virtual FName GetInteractableName() const = 0;

	UFUNCTION()
	virtual void UpdatedOverlapEnd() = 0;

	UFUNCTION()
	virtual void PlayInteractionMt() = 0;
};
