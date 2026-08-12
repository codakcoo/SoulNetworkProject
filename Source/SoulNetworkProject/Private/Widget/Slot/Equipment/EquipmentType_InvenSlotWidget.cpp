// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Equipment/EquipmentType_InvenSlotWidget.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "Interface/EquipmentInterface.h"


/*
void UEquipmentType_InvenSlotWidget::OnClicked()
{
	//if(!Item) return;
	
	//if (Item->OwingHUD->GetEquipmentSlot()->GetClass()->ImplementsInterface(UEquipmentInterface::StaticClass()))
	//{
	//	Cast<IEquipmentInterface>(Item->OwingHUD->GetEquipmentSlot())->InOutItemSlot(SlotIndex);
	//}
	//Item->OwingInventory->AddEquipItem(Item, 0, EEquipHand::Left);

}
*/

void UEquipmentType_InvenSlotWidget::OnClicked()
{
	if(!SlotData.IsSet()) return;

	/*
	*/
	if (OwnerHUD->GetEquipmentSlot()->GetClass()->ImplementsInterface(UEquipmentInterface::StaticClass()))
	{
		Cast<IEquipmentInterface>(OwnerHUD->GetEquipmentSlot())->InOutItemSlot(this);
	}
	//SlotData.OwingInventory->AddEquipItem(SlotData, 0, EEquipHand::Left);

}