// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PopupWidget.h"
#include "Widget/PopupBGWidget.h"
#include "Interface/SlateInterface.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

bool UPopupWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UPopupWidget::ConfirmClicked);
	if (CancelButton) CancelButton->OnClicked.AddDynamic(this, &UPopupWidget::CancelClicked);

	return true;
}

void UPopupWidget::SetupData(UUserWidget* InParent)
{
	if (InParent)
	{
		if (BGClass)
		{
			ParentWidget = InParent;
			BGWidget = CreateWidget<UPopupBGWidget>(this, BGClass);
			if(BGWidget) BGWidget->SetParentWidget(ParentWidget);
			AddToViewport();
		}
	}
}

void UPopupWidget::SetDescriptorText(FString InText)
{
	DescriptorText->SetText(FText::FromString(InText));
}

void UPopupWidget::AllRemoveWidget()
{
	if(BGWidget)
	{
		BGWidget->RemoveFromViewport();
		BGWidget = nullptr;
	}
	RemoveFromViewport();
}

void UPopupWidget::ConfirmClicked()
{
	if (ParentWidget->GetClass()->ImplementsInterface(USlateInterface::StaticClass()))
	{
		Cast<ISlateInterface>(ParentWidget)->ConfirmPopup();
	}
}

void UPopupWidget::CancelClicked()
{
	if (ParentWidget->GetClass()->ImplementsInterface(USlateInterface::StaticClass()))
	{
		Cast<ISlateInterface>(ParentWidget)->CancelPopup();
	}
}
