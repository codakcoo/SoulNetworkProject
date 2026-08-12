// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Slot/SlotWidget.h"
#include "SoulNetworkProject/ItemState.h"
#include "EquipmentType_SlotWidget.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API UEquipmentType_SlotWidget : public USlotWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	virtual void OnClicked() override;
	UFUNCTION()
	virtual void OnHovered() override;

	UFUNCTION()
	void SetSlotType(EItemType InSlotType, uint8 Index = 0, EEquipHand InHandType = EEquipHand::None);
	UFUNCTION()
	EItemType GetSlotType() { return SlotType; }
	UFUNCTION()
	EEquipHand GetHandType() { return HandType; }


private:
	EItemType SlotType;
	EEquipHand HandType;
};
