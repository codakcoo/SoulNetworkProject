// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/InventoryType_SlotWidget.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "Interface/InventoryInterface.h"

#include "Components/Image.h"

bool UInventoryType_SlotWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if(SelectImage) SelectImage->VisibilityDelegate.BindUFunction(this, "bSelectedEnabled");

	return true;
}

void UInventoryType_SlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//ActionType = EActionType::Off;
	//UpdatedSelect(false);
}

//Data·Î º¯Çü//
void UInventoryType_SlotWidget::OnClicked()
{
	if (!SlotData.IsSet()) return;
	
	//if (ActionType == EActionType::Off)
	if(!bSelected)
	{
		if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
		{
			Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->OnReaction(this);
		}
	}
	else
	{
		if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
		{
			Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->OffReaction(this);
		}
	}

}

void UInventoryType_SlotWidget::OnHovered()
{
	Super::OnHovered();

	if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	{
		Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->UpdatedWidgetText(this);
	}
}
