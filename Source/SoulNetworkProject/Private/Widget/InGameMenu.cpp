// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InGameMenu.h"
#include "Widget/OptionWidget.h"
#include "Widget/Slot/Inventroy/InvenType_InventorySlotWidget.h"
#include "Widget/Slot/EquipmentWidget.h"
#include "Widget/Slot/Checkpoint/StatusMenuWidget.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"
#include "SoulNetworkProject/InGameModeBase.h"
#include "SoulNetworkProject/PlayerHUD.h"

#include "Interface/MenuInterface.h"
#include "Interface/CharacterInterface.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "SoulNetworkLog.h"

#define EQUIP 0
#define INVEN 1
#define STATUS 2
#define OPTION 3
#define EXIT 4

bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	VisibilityDelegate.BindUFunction(this, "UpdateVIsibility");
	if(EquipmentButton)
	{
		EquipmentButton->OnClicked.AddDynamic(this, &UInGameMenu::OpenEquipment);
		EquipmentButton->OnHovered.AddDynamic(this, &UInGameMenu::EquipmentHovered);
		EquipmentButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");
	}
	if(InventoryButton)
	{
		InventoryButton->OnClicked.AddDynamic(this, &UInGameMenu::OpenInventory);
		InventoryButton->OnHovered.AddDynamic(this, &UInGameMenu::InventoryHovered);
		InventoryButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");
	}
	if (StatusButton)
	{
		StatusButton->OnClicked.AddDynamic(this, &UInGameMenu::OpenStatus);
		StatusButton->OnHovered.AddDynamic(this, &UInGameMenu::StatusHovered);
		StatusButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");
	}
	if (OptionButton)
	{
		OptionButton->OnClicked.AddDynamic(this, &UInGameMenu::OpenOption);
		OptionButton->OnHovered.AddDynamic(this, &UInGameMenu::OptionHovered);
		OptionButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");
	}
	if (BGButton)
	{
		BGButton->OnClicked.AddDynamic(this, &UInGameMenu::BGClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UInGameMenu::CallMainMenu);
		ExitButton->OnHovered.AddDynamic(this, &UInGameMenu::ExitHovered);
		//ExitButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");
	}

	return true;
}

/*
void UInGameMenu::CallbackSwitcher_Implementation()
{
	MenuSwitcher->SetActiveWidget(SelectWidget);
}
*/

void UInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		Owner = Player;
	}
}

//FReply UInGameMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
//{
//	if(InKeyEvent.GetKey() == B_UpperKey || InKeyEvent.GetKey() == P_UpperKey) FocusIndex+=1;
//	else if (InKeyEvent.GetKey() == B_LowerKey || InKeyEvent.GetKey() == P_LowerKey) FocusIndex -= 1;
//	//else if (InKeyEvent.GetKey() == B_UpperKey || InKeyEvent.GetKey() == P_UpperKey) FocusIndex += 1;
//	//else if (InKeyEvent.GetKey() == B_UpperKey || InKeyEvent.GetKey() == P_UpperKey) FocusIndex += 1;
//	if(FocusIndex < EQUIP) FocusIndex = EXIT;
//	else if(FocusIndex > EXIT) FocusIndex = EQUIP;
//
//
//	UpdateFocus();
//
//	return FReply::Handled();
//}
//
//FReply UInGameMenu::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
//{
//	return FReply::Handled();
//}

void UInGameMenu::CallSelectMenu()
{
	MenuSwitcher->SetActiveWidget(SelectWidget);
	MenuType = EMenuType_InGame::MainMenu;
	FocusIndex = 0;

	UpdateFocus();

	PlayAnimation(Blink);
}

void UInGameMenu::OpenEquipment()
{
	MenuSwitcher->SetActiveWidget(EquipmentMenuWidget);
	MenuType = EMenuType_InGame::EquipMenu;

	PlayAnimation(Blink);
}

void UInGameMenu::OpenInventory()
{
	MenuSwitcher->SetActiveWidget(InventoryMenuWidget);
	MenuType = EMenuType_InGame::InventoryMenu;

	PlayAnimation(Blink);
}

void UInGameMenu::OpenStatus()
{
	MenuSwitcher->SetActiveWidget(StatusMenuWidget);
	MenuType = EMenuType_InGame::StatusMenu;

	PlayAnimation(Blink);
}

void UInGameMenu::OpenOption()
{
	if (!MenuSwitcher || !OptionMenuWidget) return;
	MenuSwitcher->SetActiveWidget(OptionMenuWidget);
	MenuType = EMenuType_InGame::OptionMenu;

	if (UOptionWidget* OptionMenu = Cast<UOptionWidget>(OptionMenuWidget)) OptionMenu->SetParentWidget(this);
	UE_LOG(LogSoulUI, Warning, TEXT("OptionMenu Open"));
	

	PlayAnimation(Blink);
}

void UInGameMenu::CallMainMenu()
{
	Teardown();
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		if (Owner->GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			Cast<ICharacterInterface>(Owner->GetPlayerState())->ExitEvent();
		}

		Cast<IMenuInterface>(GetGameInstance())->Close();
		Cast<IMenuInterface>(GetGameInstance())->BackMainMenu();
	}
}

void UInGameMenu::BGClicked()
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		PH->OnOffInGame();
	}
}

bool UInGameMenu::UpdateEnabledButton()
{
	if (GetOwningPlayerPawn())
	{
		return !Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterMoving();
	}

	return false;
}

void UInGameMenu::ReverseMenu()
{
	if (MenuType == EMenuType_InGame::MainMenu)
	{
		if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			OwnerHUD->OnOffInGame();
		}
	}
	else if(MenuType == EMenuType_InGame::EquipMenu)
	{
		if(!EquipmentMenuWidget->IsActiveEquipMenu()) EquipmentMenuWidget->BackEquipMenu();
		else CallSelectMenu();
	}
	else if(MenuType == EMenuType_InGame::InventoryMenu)
	{
		//if ()
		//{
		//	InventoryMenuWidget->DisableSelectMode();
		//}
		if (InventoryMenuWidget->GetInventoryType() == EInventoryType::Action || 
				InventoryMenuWidget->GetInventoryType() == EInventoryType::Seleced)
		{
			InventoryMenuWidget->ActivateInventory();
		}
		else if (InventoryMenuWidget->GetInventoryType() == EInventoryType::Confirm)
		{
			InventoryMenuWidget->GetReactionType() == EReactionType::Single ? 
				InventoryMenuWidget->ActivateAction() :
				InventoryMenuWidget->ActivateSelected(InventoryMenuWidget->GetConfirmType(), InventoryMenuWidget->GetReactionType());
		}
		else if (InventoryMenuWidget->GetInventoryType() == EInventoryType::None)
		{
			CallSelectMenu();
		}
	}
	else if (MenuType == EMenuType_InGame::StatusMenu)
	{
		CallSelectMenu();
	}
	else if(MenuType == EMenuType_InGame::OptionMenu) OptionMenuWidget->ExitOption();
}

void UInGameMenu::SwitchMenu(bool bForward)
{
	if(MenuType == EMenuType_InGame::EquipMenu || MenuType == EMenuType_InGame::MainMenu) return;


	if (MenuType == EMenuType_InGame::InventoryMenu) InventoryMenuWidget->SwitchingMenu(bForward);
	//else if (MenuType == EMenuType_InGame::SpellMenu) SpellMenuWidget->SwitchingMenu(bForward);
	else if (MenuType == EMenuType_InGame::OptionMenu) OptionMenuWidget->SwitchingMenu(bForward);
}

void UInGameMenu::ConfirmMenu()
{
	if (MenuType == EMenuType_InGame::InventoryMenu)
	{
		if (InventoryMenuWidget->GetInventoryType() == EInventoryType::Seleced)
		{
			InventoryMenuWidget->ActivateConfirm(InventoryMenuWidget->GetConfirmType(), InventoryMenuWidget->GetReactionType());
		}
	}
}

void UInGameMenu::BindPawn(APlayerCharacter* InPawn)
{
	if(!InPawn) return;

	Owner = InPawn;	
}

ESlateVisibility UInGameMenu::UpdateVIsibility()
{
	if(!Owner || !Owner->GetHUD()) return ESlateVisibility::Hidden;

	return Owner->GetHUD()->GetActiveMenu() == EActiveMenu::InGameMenu ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
}

void UInGameMenu::ReadyEquipGradeBox()
{
	if (EquipmentMenuWidget && InventoryMenuWidget)
	{
		InventoryMenuWidget->ChangeInventorySlot(EItemType::Expendable);
	}
}

void UInGameMenu::ConfirmPopup()
{
	ConfirmMenu();
}

void UInGameMenu::CancelPopup()
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		PH->TotalReverseMenu();
	}
}

void UInGameMenu::BackgroundPopup()
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
	}
}

bool UInGameMenu::CanShowPopupWidget()
{
	bool bCan = true;

	if (MenuType == EMenuType_InGame::InventoryMenu)
	{
		if (InventoryMenuWidget->GetInventoryType() == EInventoryType::Action || 
			InventoryMenuWidget->GetInventoryType() == EInventoryType::Confirm)
		{
			return false;
		}
	}

	return bCan;
}

void UInGameMenu::UpdateFocus()
{
	StopAnimation(EquipBlink);
	StopAnimation(InvenBlink);
	StopAnimation(StatusBlink);
	StopAnimation(OptionBlink);
	StopAnimation(ExitBlink);

	EquipBG->SetVisibility(FocusIndex == EQUIP ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	InvenBG->SetVisibility(FocusIndex == INVEN ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	StatusBG->SetVisibility(FocusIndex == STATUS ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	OptionBG->SetVisibility(FocusIndex == OPTION ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	ExitBG->SetVisibility(FocusIndex == EXIT ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	if(FocusIndex == EQUIP)
	{
		EquipmentButton->SetKeyboardFocus();
		PlayAnimation(EquipBlink, 0.0f, 0);
	}
	else if (FocusIndex == INVEN)
	{
		InventoryButton->SetKeyboardFocus();
		PlayAnimation(InvenBlink, 0.0f, 0);
	}
	else if (FocusIndex == STATUS)
	{
		StatusButton->SetKeyboardFocus();
		PlayAnimation(StatusBlink, 0.0f, 0);
	}
	else if (FocusIndex == OPTION)
	{
		OptionButton->SetKeyboardFocus();
		PlayAnimation(OptionBlink, 0.0f, 0);
	}
	else if (FocusIndex == EXIT)
	{
		ExitButton->SetKeyboardFocus();
		PlayAnimation(ExitBlink, 0.0f, 0);
	}

}
