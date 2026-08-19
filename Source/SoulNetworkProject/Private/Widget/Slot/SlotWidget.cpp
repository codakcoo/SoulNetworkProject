// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/SlotWidget.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "Classes/InventoryComponent.h"

#include "SoulNetworkProject/ItemData.h"



#include "Interface/StateWidgetInterface.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"

bool USlotWidget::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	ItemButton->OnClicked.AddDynamic(this, &USlotWidget::OnClicked);
	ItemButton->OnHovered.AddDynamic(this, &USlotWidget::OnHovered);

	UpdateSlot();

	return true;
}

void USlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

//Data
void USlotWidget::SetItemSlot(FInventoryData& DataRow)
{
	SlotData = DataRow;

	ItemImage->SetBrushFromTexture(SlotData.ItemData.ItemImage);
	UpdateSlot();
}

//Data
void USlotWidget::RemoveSlot()
{
	SlotData.ResetValue();

	UpdateSlot();
}

//Data
void USlotWidget::UpdateSlot()
{
	if (!SlotData.IsSet())
	{
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
		EquipIcon->SetVisibility(ESlateVisibility::Hidden);
		ItemImage->SetBrushFromTexture(nullptr);
	
		CountBorder->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		ItemImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		EquipIcon->SetVisibility(SlotData.bEquip ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
		ItemImage->SetBrushFromTexture(SlotData.ItemData.ItemImage);
	
		if (SlotData.ItemData.ItemType == EItemType::Expendable || 
			SlotData.ItemData.ItemType == EItemType::Arrow || 
			SlotData.ItemData.ItemType == EItemType::Enhancement)
		{
			CountBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			CountBorder->SetVisibility(ESlateVisibility::Hidden);
		}
		//CountBorder->SetVisibility(SlotData.ItemData.ItemType == EItemType::Expendable ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
		CountText->SetText(FText::FromString(FString::FromInt(SlotData.ItemCount)));
	}
}

void USlotWidget::OnHovered()
{
	if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UStateWidgetInterface::StaticClass()))
	{
		Cast<IStateWidgetInterface>(OwnerHUD->GetInventorySlot())->UpdateStateMenu(this);
	}
}
