// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UI/WarpRowWidget.h"
#include "Widget/Slot/Checkpoint/WarpWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

bool UWarpRowWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if(RowButton)
	{
		RowButton->OnClicked.AddDynamic(this, &UWarpRowWidget::RowButtonClicked);
		RowButton->OnHovered.AddDynamic(this, &UWarpRowWidget::RowButtonHovered);
	}

	return true;
}

void UWarpRowWidget::SetupData(UUserWidget* InParentWidget, UTexture2D* InImage, FName InName, bool bAcitvate)
{
	if(!InParentWidget || !InImage || InName == NAME_None) return;

	ParentWidget = Cast<UWarpWidget>(InParentWidget);
	RowName = InName;
	WarpImage = InImage;

	WarpNameText->SetText(FText::FromName(RowName));
	SetIsEnabled(bAcitvate);
}

void UWarpRowWidget::PlayBlinkAnimation(bool bPlay)
{
	if (bPlay)
	{
		BG->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(Blink, 0.0f, 0);
	}
	else
	{
		BG->SetVisibility(ESlateVisibility::Hidden);
		StopAnimation(Blink);
	}
}

void UWarpRowWidget::RowButtonClicked()
{
	if (!ParentWidget || !WarpImage || RowName == NAME_None) return;

	ParentWidget->WarpCheckPoint(RowName);
}

void UWarpRowWidget::RowButtonHovered()
{
	if (!ParentWidget || !WarpImage || RowName == NAME_None) return;

	ParentWidget->UpdatedImage(this);
}

void UWarpRowWidget::SetApplierPoint()
{
	ArrowImage->SetVisibility(ESlateVisibility::Visible);
	SetIsEnabled(false);
}

