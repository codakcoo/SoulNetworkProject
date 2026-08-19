// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/ConfirmMenuWidget.h"
#include "Interface/InventoryInterface.h"
#include "SoulNetworkProject/PlayerHUD.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

bool UConfirmMenuWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if(ConfirmText) ConfirmText->TextDelegate.BindUFunction(this, "UpdateText");

	if(AcceptButton) AcceptButton->OnClicked.AddDynamic(this, &UConfirmMenuWidget::AcceptClicked);
	if(CancelButton) CancelButton->OnClicked.AddDynamic(this, &UConfirmMenuWidget::CancelClicked);


	return true;
}

/*
void UConfirmMenuWidget::ActiveConfirm(EConfirmType Confirm, EReactionType Reaction)
{
	AddToViewport();
	PlayAnimation(OnConfirmMenu);

	ConfirmType = Confirm;
	ReactionType = Reaction;

}
*/

void UConfirmMenuWidget::DisableConfirm()
{
	PlayAnimation(OffConfirmMenu);

	//GetWorld()->GetTimerManager().SetTimer(RemoveTimer, FTimerDelegate::CreateLambda([&]()
	//{
	//	RemoveFromViewport();
	//}), OffConfirmMenu->GetEndTime(), false);
}

FText UConfirmMenuWidget::UpdateText()
{
	FString CurrentText = "-";

	//if(ConfirmType == EConfirmType::None) return FText::FromString(CurrentText);
	//else if(ConfirmType == EConfirmType::Drop)
	//{
	//	CurrentText = "Are you sure you want to drop the item";
	//}
	//else if (ConfirmType == EConfirmType::Destroy)
	//{
	//	CurrentText = "Are you sure you want to destroy the item";
	//}

	return FText::FromString(CurrentText);
}

void UConfirmMenuWidget::AcceptClicked()
{
	//if (ReactionType == EReactionType::Multi)
	//{
	//	if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	//	{
	//		if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	//		{
	//			Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->ReactionAtOnce();
	//		}
	//	}
	//}
	//else if (ReactionType == EReactionType::Single)
	//{
	//	if (ConfirmType == EConfirmType::Drop)
	//	{
	//		if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	//		{
	//			if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	//			{
	//				Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->DropReaction();
	//			}
	//		}
	//	}
	//	else if (ConfirmType == EConfirmType::Destroy)
	//	{
	//		if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	//		{
	//			if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	//			{
	//				Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->DestroyReaction();
	//			}
	//		}
	//	}
	//
	//}
}

void UConfirmMenuWidget::CancelClicked()
{
	//if (ReactionType == EReactionType::Multi)
	//{
	//	if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	//	{
	//		if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	//		{
	//			Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->CancelConfirm();
	//		}
	//	}
	//}
	//else if (ReactionType == EReactionType::Single)
	//{
	//	if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	//	{
	//		if (OwnerHUD->GetInventorySlot()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	//		{
	//			Cast<IInventoryInterface>(OwnerHUD->GetInventorySlot())->BackActionMenu();
	//		}
	//	}
	//}
}
