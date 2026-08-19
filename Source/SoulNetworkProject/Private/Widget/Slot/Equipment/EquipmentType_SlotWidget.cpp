// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Equipment/EquipmentType_SlotWidget.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "Interface/EquipmentInterface.h"


void UEquipmentType_SlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEquipmentType_SlotWidget::OnClicked()
{
	if (OwnerHUD->GetEquipmentSlot()->GetClass()->ImplementsInterface(UEquipmentInterface::StaticClass()))
	{
		Cast<IEquipmentInterface>(OwnerHUD->GetEquipmentSlot())->OnSelectMenu(GetSlotType());
	}
}

void UEquipmentType_SlotWidget::OnHovered()
{
	Super::OnHovered();
	if (OwnerHUD->GetEquipmentSlot()->GetClass()->ImplementsInterface(UEquipmentInterface::StaticClass()))
	{
		Cast<IEquipmentInterface>(OwnerHUD->GetEquipmentSlot())->UpdateWidgetText(this);
	}
}

void UEquipmentType_SlotWidget::SetSlotType(EItemType InSlotType, uint8 Index, EEquipHand InHandType)
{ 
	SlotType = InSlotType;
	SlotIndex = Index;
	HandType = InHandType;
}	
