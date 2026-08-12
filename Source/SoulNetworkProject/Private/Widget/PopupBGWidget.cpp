// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PopupBGWidget.h"
#include "Widget/PopupWidget.h"
#include "Interface/SlateInterface.h"

#include "Components/Button.h"

bool UPopupBGWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if(BGButton) BGButton->OnClicked.AddDynamic(this, &UPopupBGWidget::BGOnClicked);

	return true;
}

void UPopupBGWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UPopupBGWidget::SetParentWidget(UUserWidget* InParent)
{
	if (InParent)
	{
		ParentWidget = InParent;
		AddToViewport();

		return true;
	}
	return false;
}

bool UPopupBGWidget::SetCurrSlot(UUserWidget* InSlot)
{
	if (InSlot)
	{
		return true;
	}

	return false;
}

void UPopupBGWidget::BGOnClicked()
{
	if (ParentWidget->GetClass()->ImplementsInterface(USlateInterface::StaticClass()))
	{
		Cast<ISlateInterface>(ParentWidget)->BackgroundPopup();
	}
}
