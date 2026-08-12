// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Checkpoint/CPSlotWidget.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "Interface/StorageInventoryInterface.h"


void UCPSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCPSlotWidget::OnClicked()
{
	if (OwnerHUD->GetStorageSlot()->GetClass()->ImplementsInterface(UStorageInventoryInterface::StaticClass()))
	{
		Cast<IStorageInventoryInterface>(OwnerHUD->GetStorageSlot())->UpdatedItemSlot(this);
	}
}

void UCPSlotWidget::OnHovered()
{
	Super::OnHovered();
	if (OwnerHUD->GetStorageSlot()->GetClass()->ImplementsInterface(UStorageInventoryInterface::StaticClass()))
	{
		Cast<IStorageInventoryInterface>(OwnerHUD->GetStorageSlot())->UpdateItemText(this);
	}
}
