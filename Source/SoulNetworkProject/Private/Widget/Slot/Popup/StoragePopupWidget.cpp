// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Popup/StoragePopupWidget.h"
#include "Widget/Slot/SlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Engine/UserInterfaceSettings.h"

bool UStoragePopupWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;
	
	if (UpButton) UpButton->OnClicked.AddDynamic(this, &UStoragePopupWidget::CountUpCkicked);
	if (DownButton) DownButton->OnClicked.AddDynamic(this, &UStoragePopupWidget::CountDownCkicked);
	if (Up10Button) Up10Button->OnClicked.AddDynamic(this, &UStoragePopupWidget::CountUp10Ckicked);
	if (Down10Button) Down10Button->OnClicked.AddDynamic(this, &UStoragePopupWidget::CountDown10Ckicked);

	if (CountText)
	{
		CountText->TextDelegate.BindDynamic(this, &UStoragePopupWidget::UpdatedCountText);
		CountText->SynchronizeProperties();
	}

	return true;
}

void UStoragePopupWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UStoragePopupWidget::CountUpCkicked()
{
	if (CurrentSlot->SlotData.IsSet())
	{
		CurrentCount++;

		if(CurrentCount > CurrentSlot->SlotData.ItemCount) CurrentCount = 1;
		else if(CurrentCount < 1) CurrentCount = CurrentSlot->SlotData.ItemCount;
	}
}

void UStoragePopupWidget::CountDownCkicked()
{
	if (CurrentSlot->SlotData.IsSet())
	{
		CurrentCount -= 1;

		if (CurrentCount > CurrentSlot->SlotData.ItemCount) CurrentCount = 1;
		else if (CurrentCount < 1) CurrentCount = CurrentSlot->SlotData.ItemCount;
	}
}

void UStoragePopupWidget::CountUp10Ckicked()
{
	if (CurrentSlot->SlotData.IsSet())
	{
		CurrentCount += 10;

		if (CurrentCount > CurrentSlot->SlotData.ItemCount) CurrentCount = CurrentSlot->SlotData.ItemCount;
		else if (CurrentCount < 1) CurrentCount = 1;
	}
}

void UStoragePopupWidget::CountDown10Ckicked()
{
	if (CurrentSlot->SlotData.IsSet())
	{
		CurrentCount -= 10;

		if (CurrentCount > CurrentSlot->SlotData.ItemCount) CurrentCount = CurrentSlot->SlotData.ItemCount;
		else if (CurrentCount < 1) CurrentCount = 1;
	}
}

FText UStoragePopupWidget::UpdatedCountText()
{
	FString OutputText = TEXT("0 / 0");

	if (CurrentSlot->SlotData.IsSet())
	{
		OutputText = FString::FromInt(CurrentCount) + TEXT(" / ") + FString::FromInt(CurrentSlot->SlotData.ItemCount);

		return  FText::FromString(OutputText);
	}

	return  FText::FromString(OutputText);
}

void UStoragePopupWidget::SetSlotData(USlotWidget* InSlot)
{
	if (!InSlot || !InSlot->SlotData.IsSet()) AllRemoveWidget();
	else
	{
		PlayAnimation(OnAnimation);

		CurrentCount = 1;
		CurrentSlot = InSlot;
	}
}

void UStoragePopupWidget::SetPositionBySlot()
{
	if (CurrentSlot)
	{
		FVector2D LocalCoordinate = FVector2D(0);
		FVector2D PixelPosition;
		FVector2D ViewportPosition;

		FVector2D ViewportSize;
		float ViewportScale = 1.0f;

		UWorld* World = GEngine->GetWorldFromContextObject(GetWorld(), EGetWorldErrorMode::LogAndReturnNull);
		if (World && World->IsGameWorld())
		{
			if (UGameViewportClient* ViewportClient = World->GetGameViewport())
			{
				ViewportClient->GetViewportSize(ViewportSize);
				ViewportScale = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
			}
		}

		USlateBlueprintLibrary::LocalToViewport(GetWorld(), CurrentSlot->GetCachedGeometry(), LocalCoordinate, PixelPosition, ViewportPosition);
		ViewportPosition.X += CurrentSlot->GetDesiredSize().X;
		if (ViewportPosition.Y + GetDesiredSize().Y > ViewportSize.Y)
		{
			ViewportPosition.Y += (ViewportSize.Y - (ViewportPosition.Y + GetDesiredSize().Y));
		}
		ViewportPosition *= ViewportScale;


		SetPositionInViewport(ViewportPosition);
	}
}
