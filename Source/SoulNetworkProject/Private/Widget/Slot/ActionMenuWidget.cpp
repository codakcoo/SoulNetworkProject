// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/ActionMenuWidget.h"
#include "Widget/Slot/InventoryType_SlotWidget.h"
#include "Widget/PopupBGWidget.h"
#include "Classes/InventoryComponent.h"
#include "Classes/PlayerCharacter.h"
#include "Interface/InventoryInterface.h"


#include "SoulNetworkProject/PlayerHUD.h"
#include "SoulNetworkProject/ItemData.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Engine/UserInterfaceSettings.h"


bool UActionMenuWidget::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	if(UseButton) UseButton->OnClicked.AddDynamic(this, &UActionMenuWidget::UseClicked);
	if(DropButton) DropButton->OnClicked.AddDynamic(this, &UActionMenuWidget::DropClicked);
	if(DestroyButton) DestroyButton->OnClicked.AddDynamic(this, &UActionMenuWidget::DestroyClicked);
	if(DropAtOnceButton) DropAtOnceButton->OnClicked.AddDynamic(this, &UActionMenuWidget::DropAtOnceClicked);
	if(DestroyAtOnceButton)	DestroyAtOnceButton->OnClicked.AddDynamic(this, &UActionMenuWidget::DestroyAtOnceClicked);

	//if(DescriptorText) DescriptorText->TextDelegate.BindUFunction(this, "UpdateExplanationText");

	//if (AcceptButton) AcceptButton->OnClicked.AddDynamic(this, &UActionMenuWidget::AcceptClicked);
	//if (CancelButton) CancelButton->OnClicked.AddDynamic(this, &UActionMenuWidget::CancelClicked);

	if (IncreaseButton) IncreaseButton->OnClicked.AddDynamic(this, &UActionMenuWidget::IncreaseClicked);
	if (DecreaseButton) DecreaseButton->OnClicked.AddDynamic(this, &UActionMenuWidget::DecreaseClicked);


	return true;
}

void UActionMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuSwitcher->SetActiveWidget(ActionMenu);
}

//Data�� ����//
void UActionMenuWidget::UpdateActionMenu(USlotWidget* InSlot)
{
	if (!InSlot)
	{
		CurrentSlot = nullptr;

		float AddPositionY = GetDesiredSize().Y;
		//PlayAnimation(OffAction);

		//GetWorld()->GetTimerManager().SetTimer(ActionHandle, FTimerDelegate::CreateLambda([&]
		//{
		//	RemoveFromViewport();
		//}), OffAction->GetEndTime(), false);

		AllRemoveWidget();
	}
	else
	{
		PlayAnimation(OnAction);
		//AddToViewport();



		CurrentSlot = InSlot;


		

		UseButton->SetIsEnabled(CurrentSlot->SlotData.ItemData.bUseOption);
		DropButton->SetIsEnabled(CurrentSlot->SlotData.ItemData.bDropOption);
		DestroyButton->SetIsEnabled(CurrentSlot->SlotData.ItemData.bDestroyOption);
		DropAtOnceButton->SetIsEnabled(CurrentSlot->SlotData.ItemData.bDropAtOnceOption);
		DestroyAtOnceButton->SetIsEnabled(CurrentSlot->SlotData.ItemData.bDestroyAtOnceOption);
	}
}

//ī��Ʈ �޴� ������Ʈ//
void UActionMenuWidget::UpdatedCountWidget()
{
	FString CurrentString = FString::FromInt(CurrentCount) + " / " + FString::FromInt(CurrentSlot->SlotData.ItemCount);
	CountText->SetText(FText::FromString(CurrentString));

	IncreaseButton->SetIsEnabled(CurrentCount >= CurrentSlot->SlotData.ItemCount ? false : true);
	DecreaseButton->SetIsEnabled(CurrentCount <= 1 ? false : true);
}
///////////////////////

//â ����, ���� �Լ�//
void UActionMenuWidget::VisibilityActionMenu()
{
	PlayAnimation(OnAction);
	if(BGWidget) BGWidget->SetVisibility(ESlateVisibility::Visible);
}

void UActionMenuWidget::HiddenActionMenu()
{
	PlayAnimation(OffAction);
	if(BGWidget) BGWidget->SetVisibility(ESlateVisibility::Hidden);
}
//////////////////////////


//����ϱ�//
void UActionMenuWidget::UseClicked()
{
	if(!CurrentSlot->SlotData.IsSet()) return;

	//if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	//{
	//	if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	//	{
	//		Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->UseReaction();
	//	}
	//}
	if (ParentWidget->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	{
		Cast<IInventoryInterface>(ParentWidget)->UseReaction();
	}
}

//������//
void UActionMenuWidget::DropClicked()
{
	if (!CurrentSlot) return;

	if (CurrentSlot->SlotData.ItemData.ItemType == EItemType::Expendable || CurrentSlot->SlotData.ItemData.ItemType == EItemType::Arrow)
	{
		MenuSwitcher->SetActiveWidget(CountMenu);
		PlayAnimation(SwitchCountMenu);

		CurrentCount = 1;
		UpdatedCountWidget();
		ActionType = EActionType::Drop;
	}
	else
	{
		//if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		//{
		//	if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
		//	{
		//		Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->ActiveDropMenu();
		//	}
		//}
		if (ParentWidget->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
		{
			Cast<IInventoryInterface>(ParentWidget)->ActiveDropMenu();
		}
	}
}

//�ı��ϱ�//
void UActionMenuWidget::DestroyClicked()
{
	if(!CurrentSlot) return;

	if (CurrentSlot->SlotData.ItemData.ItemType == EItemType::Expendable || CurrentSlot->SlotData.ItemData.ItemType == EItemType::Arrow)
	{
		MenuSwitcher->SetActiveWidget(CountMenu);
		PlayAnimation(SwitchCountMenu);

		CurrentCount = 1;
		UpdatedCountWidget();
		ActionType = EActionType::Destroy;
	}
	else
	{
		//if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		//{
		//	if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
		//	{
		//		Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->ActiveDestroyMenu();
		//	}
		//}
		if (ParentWidget->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
		{
			Cast<IInventoryInterface>(ParentWidget)->ActiveDestroyMenu();
		}
	}
}

//�� ���� ������//
void UActionMenuWidget::DropAtOnceClicked()
{
	//if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	//{
	//	if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	//	{
	//		Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->SetDropSelectMode();
	//	}
	//}

	if (ParentWidget->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	{
		Cast<IInventoryInterface>(ParentWidget)->SetDropSelectMode();
	}
}

//�� ���� �ı��ϱ�//
void UActionMenuWidget::DestroyAtOnceClicked()
{
	//if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	//{
	//	if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	//	{
	//		Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->SetDestroySelectMode();
	//	}
	//}

	if (ParentWidget->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	{
		Cast<IInventoryInterface>(ParentWidget)->SetDestroySelectMode();
	}
}

//Text ��������Ʈ//
FText UActionMenuWidget::UpdateExplanationText()
{
	FString CurrentString = "-";
	if(ActionType == EActionType::None) return FText::FromString(CurrentString);
	else if (ActionType == EActionType::Drop)
	{
		CurrentString = "Choose the amount to drop";
	}
	else if (ActionType == EActionType::Destroy)
	{
		CurrentString = "Choose the amount to destroy";
	}
	return FText::FromString(CurrentString);
}
/////////////////////

/*
//�Һ� ������ CountMenu���� Ȯ�ΰ� ��� ��ư BindUFunction//
void UActionMenuWidget::AcceptClicked()
{
	if (ActionType == EActionType::Drop)
	{
		//InventoryComponent->DropItem(CurrentSlot->SlotData, CurrentCount);
		if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
			{
				Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->DropReaction(CurrentCount);
			}
		}
	}
	else if (ActionType == EActionType::Destroy)
	{
		//InventoryComponent->DestroyItem(CurrentSlot->SlotData, CurrentCount);

		if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
			{
				Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->DestroyReaction(CurrentCount);
			}
		}
	}

	if (CurrentSlot) CurrentSlot->OnClicked();
}

void UActionMenuWidget::CancelClicked()
{
	PlayAnimation(OnAction);
	MenuSwitcher->SetActiveWidget(ActionMenu);

}
/////////////////////////////////////////////////////////////
*/

//����, ����//
void UActionMenuWidget::IncreaseClicked()
{
	CurrentCount += 1;
	UpdatedCountWidget();
}

void UActionMenuWidget::DecreaseClicked()
{
	CurrentCount -= 1;
	UpdatedCountWidget();
}
void UActionMenuWidget::SetPositionBySlot()
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
		if(ViewportPosition.Y + GetDesiredSize().Y > ViewportSize.Y)
		{
			ViewportPosition.Y += (ViewportSize.Y - (ViewportPosition.Y + GetDesiredSize().Y));
		}
		ViewportPosition *= ViewportScale;


		SetPositionInViewport(ViewportPosition);
		//GetCachedGeometry().GetLocalSize().Y;
		//float AddPositionY = GetCachedGeometry().GetLocalSize().Y;
	}
}
//////////////
