// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UI/ItemViewWidget.h"
#include "Widget/PlayerWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

bool UItemViewWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	return true;
}

void UItemViewWidget::Setup(FInventoryData& Item)
{
	ItemImage->SetBrushFromTexture(Item.ItemData.ItemImage);

	ItemNameText->SetText(FText::FromName(Item.ItemData.ItemName));

	PlayAnimation(ViewAnim);
}
