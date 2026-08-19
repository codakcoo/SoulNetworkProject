// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/GradeSlotWidget.h"
#include "Widget/Slot/SlotWidget.h"
#include "Widget/Slot/InventoryType_SlotWidget.h"


UGradeSlotWidget::UGradeSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

bool UGradeSlotWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

		

	return true;
}

void UGradeSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SlotArray.Add(ItemSlot_0);
	SlotArray.Add(ItemSlot_1);
	SlotArray.Add(ItemSlot_2);
	SlotArray.Add(ItemSlot_3);
	SlotArray.Add(ItemSlot_4);
}

void UGradeSlotWidget::SetupRowIndex(uint8 RowIndex)
{
	Index = RowIndex;
}

void UGradeSlotWidget::UpdatedItemSlot(uint8 i, FInventoryData& Data)
{
	for (int8 j = 0; j < SlotArray.Num(); j++)//& ItemSlot : SlotArray)
	{
		if (!SlotArray[j]->SlotData.IsSet() && Data.IsSet())
		{
			SlotArray[j]->SetItemSlot(Data);
			SlotArray[j]->SetSlotIndex(i);
			return;
		}
	}
}

bool UGradeSlotWidget::CanSameType(EItemType ItemState, uint8 DataType)
{
	if(SlotStackCount() == 0) return true;
	else if(SlotStackCount() == 5) return false;
	else if (SlotStackCount() > 0 || SlotStackCount() < 5)
	{
		for (auto& Row : SlotArray)
		{
			//if (Row->SlotData.IsSet())
			//{
			//}
			
				if (GetItemEnumValue<uint8>(Row->SlotData) == DataType)
				{
					return true;
				}
		}
	}
	

	return false;
}

bool UGradeSlotWidget::CanStackData()
{
	for (auto& Row : SlotArray)
	{
		if (!Row->SlotData.IsSet())
		{
			return true;
		}
	}

	return false;
}

uint8 UGradeSlotWidget::SlotStackCount()
{
	uint8 StackCount = 0;
	for (auto& ItemSlot : SlotArray)
	{
		if(ItemSlot->SlotData.IsSet()) StackCount++;
	}

	return StackCount;
}

//���� Ȱ��ȭ, ��Ȱ��ȭ//
void UGradeSlotWidget::SetSlotEnabled(bool bActive, USlotWidget* EnabledWidget)
{
	for (auto& ItemSlot : SlotArray)
	{
		ItemSlot->SetIsEnabled(bActive);

		if (!bActive)
		{
			if(ItemSlot == EnabledWidget) ItemSlot->SetIsEnabled(true);
		}
	}
}

USlotWidget* UGradeSlotWidget::GetSlotDisabled()
{
	for (auto& ItemSlot : SlotArray)
	{
		if (ItemSlot->SlotData.IsSet() && ItemSlot->GetIsEnabled())
		{
			return ItemSlot;
		}
	}
	return nullptr;
}
////////////////////////

void UGradeSlotWidget::DiabledInheritedItem()
{
	for (auto& ItemSlot : SlotArray)
	{
		if (ItemSlot->SlotData.IsSet())
		{
			if (!ItemSlot->SlotData.ItemData.bDropOption
				|| !ItemSlot->SlotData.ItemData.bDestroyOption
				|| !ItemSlot->SlotData.ItemData.bDropAtOnceOption
				|| !ItemSlot->SlotData.ItemData.bDestroyAtOnceOption)
			{
				ItemSlot->SetIsEnabled(false);
			}
		}
	}
}

void UGradeSlotWidget::UpdatedSelectedItem(FInventoryData& SelectItem)//USlotWidget* SelectSlot)
{
	for (auto& ItemSlot : SlotArray)
	{
		if (ItemSlot->SlotData.IsSet())
		{
			if (UInventoryType_SlotWidget* SelectWidget = Cast<UInventoryType_SlotWidget>(ItemSlot))
			{
				if (ItemSlot->SlotData == SelectItem)
				{
					SelectWidget->SetSelect(true);
					break;
				}
			}
		}
	}
}
