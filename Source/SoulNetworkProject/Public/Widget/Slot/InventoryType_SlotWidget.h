// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Slot/SlotWidget.h"
#include "InventoryType_SlotWidget.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API UInventoryType_SlotWidget : public USlotWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	//UFUNCTION()
	//virtual void OnClicked() override;
	UFUNCTION()
	virtual void OnClicked() override;

	UFUNCTION()
	virtual void OnHovered() override;

	

	UFUNCTION()
	ESlateVisibility bSelectedEnabled() { return bSelected ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden; }


protected:
	UPROPERTY(meta = (BindWidget))
	class UWidget* SelectImage;
};
