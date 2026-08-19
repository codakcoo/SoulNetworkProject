// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/InventorySlotWidget.h"
#include "Widget/Slot/SlotWidget.h"
#include "Widget/Slot/GradeSlotWidget.h"
#include "Widget/Slot/ActionMenuWidget.h"
#include "Classes/InventoryComponent.h"



#include "Classes/PlayerCharacter.h"
#include "SoulNetworkProject/ItemState.h"
#include "SoulNetworkProject/ItemData.h"

#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#define NOT_EXIST FText::FromString("-");

UInventorySlotWidget::UInventorySlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UInventorySlotWidget::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	return true;
}

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		InventoryComponent = Cast<UInventoryComponent>(Player->GetInventoryComponent());
	}
}

//Data�� ����//
void UInventorySlotWidget::UpdateStateMenu(USlotWidget* ItemSlot)
{
	
}

void UInventorySlotWidget::UpdatedWidget(USlotWidget* ItemSlot)
{
	if(ItemSlot == nullptr || !ItemSlot->SlotData.IsSet())
	{
		NameText_Inven->SetText(FText::FromString("-"));
		NameText_Item->SetText(FText::FromString("-"));
		ItemImage_Item->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		NameText_Inven->SetText(FText::FromName(ItemSlot->SlotData.ItemData.ItemName));
		NameText_Item->SetText(FText::FromName(ItemSlot->SlotData.ItemData.ItemName));
		ItemImage_Item->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ItemImage_Item->SetBrushFromTexture(ItemSlot->SlotData.ItemData.ItemImage);
	}
}

void UInventorySlotWidget::ChangeInventorySlot(EItemType ItemState)
{
/*
*/
	PlayAnimation(InvenBlink);
	GradeBox->ClearChildren();
	TArray<FInventoryData*> CategoryItems;
	

	for (auto& Item : InventoryComponent->Items)
	{
		if ((uint8)Item.ItemData.ItemType == (uint8)ItemState)
		{
			CategoryItems.Add(&Item);

		}
	}
	if(CategoryItems.Num() == 0) return;

	uint8 RowIndex = 0;
	uint8 SlotIndex = 0;

	const UEnum* EnumObject = GetEnumObject(ItemState);
	if(!EnumObject) return;


	UGradeSlotWidget* CreateCurrentSlot = CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
	for (uint8 i = 0; i < EnumObject->GetMaxEnumValue(); ++i)
	{
		//for (FInventoryData* Item : CategoryItems)
		//{
		//	uint8 CurrentValue = (uint8)GetItemEnumValue(*Item);
		//
		//	//if (Item->ExpendableData.ExpendableType == EExpendableType::Potion)
		//	if (CurrentValue == i)// ||
		//		//Item->ItemData.ItemName == TEXT("HealthPotion") ||
		//		//Item->ItemData.ItemName == TEXT("ManaPotion"))
		//	{
		//		//if(Item->ExpendableData.ExpendableType == EExpendableType::Potion)
		//		if (!CreateCurrentSlot->CanStackData())
		//		{
		//			RowIndex++;
		//			CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
		//		}
		//		else if (!CreateCurrentSlot->CanSameType(ItemState, i))
		//		{
		//			RowIndex++;
		//			CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
		//		}
		//
		//
		//		if (CreateCurrentSlot)
		//		{
		//			if (CreateCurrentSlot->SlotStackCount() == 0)
		//			{
		//				GradeBox->AddChild(CreateCurrentSlot);
		//			}
		//
		//			CreateCurrentSlot->SetupRowIndex(RowIndex);
		//			CreateCurrentSlot->UpdatedItemSlot(SlotIndex, *Item);
		//			SlotIndex++;
		//			//CategoryItems.RemoveSingle(Item);
		//
		//		}
		//	}
		//}
		CategoryItems = CallbackInventorySlot(ItemState, i, CategoryItems, GradeBox, CreateCurrentSlot, RowIndex, SlotIndex);
		
		if(CategoryItems.Num() == 0) break;

	}
}

bool UInventorySlotWidget::UpdateEnabledMenu()
{
	if (GetOwningPlayerPawn())
	{
		return !Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterMoving();
	}

	return false;
}

void UInventorySlotWidget::AlReadyGradeSlot()
{
	//if (InventoryComponent->Items.Num() == 0)
	//{
	//}

	IDataInterface* Interface = Cast<IDataInterface>(GetWorld()->GetGameInstance());
	dummy.ItemData = (*FindRow<FItemData>(Interface->GetItemDataTable(), TEXT("Dummy"), "Item Data Context."));
	dummy.WeaponData = (*FindRow<FWeaponData>(Interface->GetWeaponDataTable(), TEXT("Dummy"), "Weapon Data Context."));
	
	if (dummy.IsSet())
	{
		UGradeSlotWidget* CreateCurrentSlot = CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
		if (CreateCurrentSlot)
		{
			GradeBox->AddChild(CreateCurrentSlot);
			CreateCurrentSlot->SetupRowIndex(0);
			CreateCurrentSlot->UpdatedItemSlot(0, dummy);

		}
	}
}

TArray<FInventoryData*> UInventorySlotWidget::CallbackInventorySlot(EItemType SlotType, 
	uint8 CurrIndex, TArray<FInventoryData*> CurrCategoryData, 
	UWrapBox* CurrGradeBox, UGradeSlotWidget* CurrGradeWidget, 
	uint8 RowIndex, uint8 SlotIndex)
{
	//uint8 RowIndex = 0;
	//uint8 SlotIndex = 0;

	const UEnum* EnumObject = GetEnumObject(SlotType);
	if (!EnumObject) return {};

	//UGradeSlotWidget* CreateCurrentSlot = CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
	if(CurrIndex < EnumObject->GetMaxEnumValue())
	{
		for (FInventoryData* Item : CurrCategoryData)
		{
			uint8 CurrentValue = (uint8)GetItemEnumValue(*Item);

			//if (Item->ExpendableData.ExpendableType == EExpendableType::Potion)
			if (CurrentValue == CurrIndex)// ||
			//	Item->ItemData.ItemName == TEXT("HealthPotion") ||
			//	Item->ItemData.ItemName == TEXT("ManaPotion"))
			{
				//if(Item->ExpendableData.ExpendableType == EExpendableType::Potion)
				if (!CurrGradeWidget->CanStackData())
				{
					RowIndex++;
					CurrGradeWidget = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
				}
				else if (!CurrGradeWidget->CanSameType(SlotType, CurrIndex))
				{
					RowIndex++;
					CurrGradeWidget = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
				}


				if (CurrGradeWidget)
				{
					if (CurrGradeWidget->SlotStackCount() == 0)
					{
						CurrGradeBox->AddChild(CurrGradeWidget);
					}

					CurrGradeWidget->SetupRowIndex(RowIndex);
					CurrGradeWidget->UpdatedItemSlot(SlotIndex, *Item);
					SlotIndex++;
					CurrCategoryData.RemoveSingle(Item);

					CallbackInventorySlot(SlotType, CurrIndex, CurrCategoryData, CurrGradeBox, CurrGradeWidget, RowIndex, SlotIndex);
					break;
				}
			}
		}
	}

	return CurrCategoryData;
}
