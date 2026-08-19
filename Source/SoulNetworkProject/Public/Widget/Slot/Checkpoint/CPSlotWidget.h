// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Slot/SlotWidget.h"
#include "CPSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UCPSlotWidget : public USlotWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
		virtual void OnClicked() override;
	UFUNCTION()
		virtual void OnHovered() override;
};
