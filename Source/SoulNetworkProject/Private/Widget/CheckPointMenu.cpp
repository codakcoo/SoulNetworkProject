// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CheckPointMenu.h"
#include "Widget/OptionWidget.h"
#include "Widget/Slot/Checkpoint/CPInventorySlotWidget.h"
#include "Widget/Slot/Checkpoint/LevelupWidget.h"
#include "Widget/Slot/Checkpoint/PotionWidget.h"
#include "Widget/Slot/Checkpoint/WarpWidget.h"
#include "Widget/Slot/Spell/SpellType_InventorySlotWidget.h"

#include "Classes/PlayerCharacter.h"
#include "SoulNetworkProject/InGameModeBase.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "Interface/SaveInterface.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

#define LEVEL 0
#define POTION 1
#define STORAGE 2
#define SPELL 3
#define CLOSE 4

bool UCheckPointMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	VisibilityDelegate.BindUFunction(this, "UpdateVIsibility");
	if (WarpButton)
	{
		WarpButton->OnClicked.AddDynamic(this, &UCheckPointMenu::OpenWarpMenu);
	}
	if(LevelButton)
	{
		LevelButton->OnClicked.AddDynamic(this, &UCheckPointMenu::OpenLevelMenu);
		//LevelButton->OnHovered.AddDynamic(this, &UCheckPointMenu::LevelHovered);

	}
	if(PotionButton)
	{
		PotionButton->OnClicked.AddDynamic(this, &UCheckPointMenu::OpenPotionMenu);
		//PotionButton->OnHovered.AddDynamic(this, &UCheckPointMenu::PotionHovered);

	}
	if(StorageButton)
	{
		StorageButton->OnClicked.AddDynamic(this, &UCheckPointMenu::OpenStorageMenu);
		//StorageButton->OnHovered.AddDynamic(this, &UCheckPointMenu::StorageHovered);

	}
	if(SpellButton)
	{
		SpellButton->OnClicked.AddDynamic(this, &UCheckPointMenu::OpenSpellMenu);
		//SpellButton->OnHovered.AddDynamic(this, &UCheckPointMenu::SpellHovered);

	}
	if(CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UCheckPointMenu::CloseMenu);
		//CloseButton->OnHovered.AddDynamic(this, &UCheckPointMenu::CloseHovered);

	}
	if(BGButton)
	{
		BGButton->OnClicked.AddDynamic(this, &UCheckPointMenu::CloseMenu);
	}
	//EquipmentButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");
	//InventoryButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");
	//SpellButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");
	//OptionButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");
	//ExitButton->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledButton");

	return true;
}

/*
void UCheckPointMenu::CallbackSwitcher_Implementation()
{
	MenuSwitcher->SetActiveWidget(SelectWidget);
}
*/

void UCheckPointMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		Owner = Player;
		PotionWidget->SetParentWidget(this);
		WarpWidget->SetParentWidget(this);
	}
}

void UCheckPointMenu::CallSelectMenu()
{
	MenuSwitcher->SetActiveWidget(SelectMenuWidget);
	MenuType = EMenuType_CP::MainMenu;

	PlayAnimation(Blink);

	FocusIndex = 0;
	//UpdateFocus();
	//PlaySelectAnim();
}

void UCheckPointMenu::OpenWarpMenu()
{
	MenuSwitcher->SetActiveWidget(WarpWidget);
	MenuType = EMenuType_CP::Warp;
	//LevelupWidget->SetupWidget();
	//PlayAnimation(EquipBlink);
	WarpWidget->SetupMenu();
	PlayAnimation(Blink);
}

void UCheckPointMenu::OpenLevelMenu()
{
	MenuSwitcher->SetActiveWidget(LevelupWidget);
	MenuType = EMenuType_CP::Levelup;
	LevelupWidget->SetupWidget();
	//PlayAnimation(EquipBlink);
	PlayAnimation(Blink);
}

void UCheckPointMenu::OpenPotionMenu()
{
	MenuSwitcher->SetActiveWidget(PotionWidget);
	MenuType = EMenuType_CP::Potion;
	PotionWidget->CallSelectMenu();
	//PlayAnimation(InventoryBlink);
	PlayAnimation(Blink);

}

void UCheckPointMenu::OpenStorageMenu()
{
	MenuSwitcher->SetActiveWidget(StorageWidget);
	MenuType = EMenuType_CP::Storage;

	//PlayAnimation(SpellBlink);
	StorageWidget->SetupContentMenu();
	PlayAnimation(Blink);

}

void UCheckPointMenu::OpenSpellMenu()
{
	MenuSwitcher->SetActiveWidget(SpellWidget);
	MenuType = EMenuType_CP::Spell;
	SpellWidget->CallFirstMenu();
	PlayAnimation(Blink);
}

void UCheckPointMenu::CloseMenu()
{
	if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		OwnerHUD->OnOffInGame();
	}
}

bool UCheckPointMenu::UpdateEnabledButton()
{
	if (GetOwningPlayerPawn())
	{
		return !Cast<APlayerCharacter>(GetOwningPlayerPawn())->GetCharacterMoving();
	}

	return false;
}

void UCheckPointMenu::ReverseMenu()
{
	if (MenuType == EMenuType_CP::MainMenu)
	{
		if (APlayerHUD* OwnerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			OwnerHUD->OnOffInGame();

		}
	}
	else if (MenuType == EMenuType_CP::Warp)
	{
		CallSelectMenu();
	}
	else if (MenuType == EMenuType_CP::Levelup)
	{
		if (LevelupWidget->GetMenuType() == ELevelType::Popup)
		{
			LevelupWidget->ClearPopupWidget();
		}
		else
		{
			//LevelupWidget->ResetWidget();
			CallSelectMenu();
		}
	}
	else if (MenuType == EMenuType_CP::Potion)
	{
		if (PotionWidget->GetMenuType() == EPotionMenuType::Select)
		{
			CallSelectMenu();
		}
		else
		{
			PotionWidget->CallSelectMenu();
		}
	}
	else if (MenuType == EMenuType_CP::Storage)
	{
		if (StorageWidget->GetInventoryType() == EInventoryType_CP::None)
		{
			CallSelectMenu();
		}
		else if (StorageWidget->GetInventoryType() == EInventoryType_CP::Confirm)
		{
			StorageWidget->CancelPopup();
		}
	}
	else if (MenuType == EMenuType_CP::Spell)
	{
		if (SpellWidget->GetMenuType() == EMenuType_Spell::Select)
		{
			SpellWidget->BackEquipSlot();
		}
		else if (SpellWidget->GetMenuType() == EMenuType_Spell::Equip)
		{
			CallSelectMenu();
		}
	}
}

void UCheckPointMenu::SwitchMenu(bool bForward)
{
	if (MenuType == EMenuType_CP::Storage) StorageWidget->SwitchingMenu(bForward);
	else if (MenuType == EMenuType_CP::Spell) SpellWidget->SwitchingMenu(bForward);
}

void UCheckPointMenu::ConfirmMenu()
{
	if(MenuType == EMenuType_CP::Storage)
	{
		StorageWidget->SwitchActiveMenu();
	}
}

void UCheckPointMenu::BindPawn(APlayerCharacter* InPawn)
{
	if (!InPawn) return;

	Owner = InPawn;

	//EquipmentMenuWidget->ChangeInventorySlot(EItemType::Weapon);
	//InventoryMenuWidget->ChangeInventorySlot(EItemType::Expendable);
}

ESlateVisibility UCheckPointMenu::UpdateVIsibility()
{
	if (!Owner || !Owner->GetHUD()) return ESlateVisibility::Hidden;

	return Owner->GetHUD()->GetActiveMenu() == EActiveMenu::CheckPointMenu ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
}

void UCheckPointMenu::ReadyEquipGradeBox()
{
	//if (EquipmentMenuWidget && InventoryMenuWidget)
	//{
	//	//EquipmentMenuWidget->ChangeInventorySlot(EItemType::Expendable);
	//	InventoryMenuWidget->ChangeInventorySlot(EItemType::Expendable);
	//	//EquipmentMenuWidget->AlReadyGradeSlot();//ChangeInventorySlot(EItemType::Expendable);
	//	//InventoryMenuWidget->AlReadyGradeSlot();//ChangeInventorySlot(EItemType::Expendable);
	//
	//}
}

void UCheckPointMenu::ConfirmPopup()
{
}

void UCheckPointMenu::CancelPopup()
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		PH->TotalReverseMenu();
	}
}

void UCheckPointMenu::BackgroundPopup()
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
	}
}

bool UCheckPointMenu::CanShowPopupWidget()
{
	bool bCan = true;

	if (MenuType == EMenuType_CP::Storage)
	{
		if (StorageWidget->GetInventoryType() == EInventoryType_CP::Confirm)
		{
			bCan = false;
		}
	}

	return bCan;
}

void UCheckPointMenu::UpdateFocus()
{
	StopAnimation(LevelBlink);
	StopAnimation(PotionBlink);
	StopAnimation(StorageBlink);
	StopAnimation(SpellBlink);
	StopAnimation(CloseBlink);

	LevelBG->SetVisibility(FocusIndex == LEVEL ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	PotionBG->SetVisibility(FocusIndex == POTION ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	StorageBG->SetVisibility(FocusIndex == STORAGE ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	SpellBG->SetVisibility(FocusIndex == SPELL ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	CloseBG->SetVisibility(FocusIndex == CLOSE ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	if (FocusIndex == LEVEL)	PlayAnimation(LevelBlink, 0.0f, 0);
	else if (FocusIndex == POTION) PlayAnimation(PotionBlink, 0.0f, 0);
	else if (FocusIndex == STORAGE) PlayAnimation(StorageBlink, 0.0f, 0);
	else if (FocusIndex == SPELL) PlayAnimation(SpellBlink, 0.0f, 0);
	else if (FocusIndex == CLOSE) PlayAnimation(CloseBlink, 0.0f, 0);
}

