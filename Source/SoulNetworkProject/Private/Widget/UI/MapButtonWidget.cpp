// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UI/MapButtonWidget.h"
#include "Widget/Slot/CheckPoint/WarpWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

bool UMapButtonWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if(MapButton) MapButton->OnClicked.AddDynamic(this, &UMapButtonWidget::MapButtonClicked);

	return true;
}

void UMapButtonWidget::SetupData(UUserWidget* InParentWidget, UTexture2D* InImage, FName InName)
{
	if(!InParentWidget || !InImage || InName == NAME_None) return;

	ParentWidget = Cast<UWarpWidget>(InParentWidget);
	MapName = InName;

	MapImage->SetBrushFromTexture(InImage);
}

void UMapButtonWidget::MapButtonClicked()
{
	if(!ParentWidget) return;

	ParentWidget->UpdatedWidget(MapName.ToString());
}
