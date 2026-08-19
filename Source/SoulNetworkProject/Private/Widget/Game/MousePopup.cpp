// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Game/MousePopup.h"
#include "Widget/Slot/SlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Engine/UserInterfaceSettings.h"

bool UMousePopup::Initialize()
{
	bool bSuccess = Super::Initialize();
	if (!bSuccess) return false;



	return true;
}

void UMousePopup::NativeConstruct()
{
	Super::NativeConstruct();
}

FText UMousePopup::UpdatedCountText()
{
	FString OutputText = TEXT("0 / 0");

	//{
	//	OutputText = FString::FromInt(CurrentCount) + TEXT(" / ") + FString::FromInt(CurrentSlot->SlotData.ItemCount);
	//
	//	return  FText::FromString(OutputText);
	//}

	return  FText::FromString(OutputText);
}

void UMousePopup::SetSlotData()
{
	//if (!InSlot || !InSlot->SlotData.IsSet()) AllRemoveWidget();
	//else
	//{
	//
	//	CurrentCount = 1;
	//}
		PlayAnimation(OnAnimation);
}

void UMousePopup::SetPositionBySlot()
{
	if(GetWorld() && GetWorld()->GetFirstPlayerController())
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

		GetWorld()->GetFirstPlayerController()->GetMousePosition(ViewportPosition.X, ViewportPosition.Y);
		if (ViewportPosition.X + GetDesiredSize().X / 2.0f > ViewportSize.X)
		{
			ViewportPosition.X += (ViewportSize.X - (ViewportPosition.X + GetDesiredSize().X / 2.0f));
		
		}
		if (ViewportPosition.Y + GetDesiredSize().Y / 2.0f > ViewportSize.Y)
		{
			ViewportPosition.Y += (ViewportSize.Y - (ViewportPosition.Y + GetDesiredSize().Y / 2.0f));
		}

		SetPositionInViewport(ViewportPosition);
	}
}
