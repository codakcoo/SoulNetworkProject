// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Slot/SlotWidget.h"
#include "SpellType_InvenSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API USpellType_InvenSlotWidget : public USlotWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void OnHovered() override;
	UFUNCTION()
	virtual void OnClicked() override;
};
