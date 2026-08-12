// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Spell/SpellType_InvenSlotWidget.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "Interface/SpellEquipmentInterface.h"

void USpellType_InvenSlotWidget::OnHovered()
{
	if (!SlotData.IsSet()) return;

	/*
	*/
	if (OwnerHUD->GetSpellSlot()->GetClass()->ImplementsInterface(USpellEquipmentInterface::StaticClass()))
	{
		Cast<ISpellEquipmentInterface>(OwnerHUD->GetSpellSlot())->UpdateInvenWidgetText(this);
	}
	//SlotData.OwingInventory->AddEquipItem(SlotData, 0, EEquipHand::Left);
}

void USpellType_InvenSlotWidget::OnClicked()
{
	if (!SlotData.IsSet()) return;
	/*
	*/
	if (OwnerHUD->GetSpellSlot()->GetClass()->ImplementsInterface(USpellEquipmentInterface::StaticClass()))
	{
		Cast<ISpellEquipmentInterface>(OwnerHUD->GetSpellSlot())->InOutItemSlot(this);
	}
	//SlotData.OwingInventory->AddEquipItem(SlotData, 0, EEquipHand::Left);
}
