// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/UseItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


//Data로 변형//
void UUseItemWidget::SwitchingUseItem(FInventoryData& UseItem, bool bWidgetAnim)
{
	//if (UseItem) AddSlotItem(UseItem);
	//else RemoveSlotItem();
	//
	//if (bWidgetAnim)PlaySlotAnim();
}


//Data로 변형//
void UUseItemWidget::AddSlotItem(FInventoryData UseItem)
{
	SlotData = UseItem;
	UpdateSlot();
}


//Data로 변형//
void UUseItemWidget::RemoveSlotItem()
{
	SlotData.ResetValue();
	UpdateSlot();
}


//Data로 변형//
void UUseItemWidget::UpdateSlot()
{
	if (SlotData.IsSet())
	{
		ItemImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ItemImage->SetBrushFromTexture(SlotData.ItemData.ItemImage);
	
		//if (SlotData.ItemData.ItemType != EItemType::Expendable) CountBorder->SetVisibility(ESlateVisibility::Hidden);
		//else
		//{
		//	CountBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		//	CountText->SetText(FText::FromString(FString::FromInt(SlotData.ItemCount)));
		//}

		if (SlotData.ItemData.ItemType == EItemType::Expendable || SlotData.ItemData.ItemType == EItemType::Arrow)
		{
			CountBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			CountText->SetText(FText::FromString(FString::FromInt(SlotData.ItemCount)));
		}
		else
		{
			CountBorder->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
		CountBorder->SetVisibility(ESlateVisibility::Hidden);

		//GEngine->AddOnScreenDebugMessage(-1,3.f)
	}
}

void UUseItemWidget::PlaySlotAnim()
{
	UGameplayStatics::PlaySound2D(GetWorld(), BlinkSound);
	PlayAnimation(BlinkSlot);
}
