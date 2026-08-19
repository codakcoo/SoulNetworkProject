// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Widget/PlayerWidget.h"
#include "Widget/InGameMenu.h"
#include "Widget/CheckPointMenu.h"
#include "Widget/CinematicWidget.h"

#include "Widget/Slot/Inventroy/InvenType_InventorySlotWidget.h"
#include "Widget/Slot/Spell/SpellType_InventorySlotWidget.h"
#include "Widget/Slot/Checkpoint/CPInventorySlotWidget.h"
#include "Widget/Slot/EquipmentWidget.h"
#include "Widget/Slot/UseItemWidget.h"
#include "Widget/Game/MousePopup.h"
#include "Widget/UI/InteractionWidget.h"
#include "Classes/PlayerCharacter.h"


#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerHUD::APlayerHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerWidgetClass_C (TEXT("WidgetBlueprint'/Game/Widget/PlayerWidget_BP.PlayerWidget_BP_C'"));
	if (PlayerWidgetClass_C.Succeeded())
	{
		PlayerWidgetClass = PlayerWidgetClass_C.Class;
	}
}

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	
}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<APlayerCharacter>(GetOwningPawn());
}

void APlayerHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APlayerHUD::UpdatedState()
{
	PlayerWidget->InteractionEnabled();
}

void APlayerHUD::SetupInteractionBox(bool bVisible, FText InText)
{
	if(InText.ToString() == TEXT("None")) return;

	if (bVisible)
	{
		PlayerWidget->OnInteractionBox();
		PlayerWidget->SetInteractionText(InText);
	}
	else if (!bVisible)
	{
		PlayerWidget->OffInteractionBox();
	}
}

void APlayerHUD::ShowItemViewList(FInventoryData& Item)
{
	PlayerWidget->RefreshItemViewList(Item);
}

void APlayerHUD::BindBossWidget(bool bVisible)
{
	PlayerWidget->SwitchBossBox(bVisible);
}

void APlayerHUD::OnOffInGame()
{
	//if(!Owner || Owner->GetStartingStand()) return;

	if (MousePopupWidget)
	{
		return;
	}

	if (ActiveMenu == EActiveMenu::PlayerMenu)
	{
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;

		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);

		Con->SetInputMode(InputModeData);
		Con->bShowMouseCursor = true;
		
		
		ActiveMenu = EActiveMenu::InGameMenu;
		InGameWidget->CallSelectMenu();
		
	}
	else if (ActiveMenu == EActiveMenu::InGameMenu)
	{
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;

		FInputModeGameOnly InputModeData;

		Con->SetInputMode(InputModeData);
		Con->bShowMouseCursor = false;

		ActiveMenu = EActiveMenu::PlayerMenu;
		PlayerWidget->PlayWidgetAnim();
	}
	else if (ActiveMenu == EActiveMenu::Popup)
	{
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con || !Owner) return;


		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);

		Owner->ClearPopupWidget();
		Con->SetInputMode(InputModeData);
		Con->bShowMouseCursor = true;

		ActiveMenu = EActiveMenu::InGameMenu;
		InGameWidget->CallSelectMenu();
	}
	else if (ActiveMenu == EActiveMenu::CheckPointMenu)
	{
		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;

		FInputModeGameOnly InputModeData;

		Con->SetInputMode(InputModeData);
		Con->bShowMouseCursor = false;

		if(Owner->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) Owner->Server_InteractionMontage(GetupMt, NAME_None);

		ActiveMenu = EActiveMenu::PlayerMenu;
		PlayerWidget->PlayWidgetAnim();
	}
}

void APlayerHUD::SetupMenus(APlayerCharacter* InPawn)
{
	PlayerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);
	if (PlayerWidget)
	{
		PlayerWidget->AddToViewport();
	}

	InGameWidget = CreateWidget<UInGameMenu>(GetWorld(), InGameWidgetClass);
	if (InGameWidget)
	{
		InGameWidget->AddToViewport();
	}

	CheckPointWidget = CreateWidget<UCheckPointMenu>(GetWorld(), CheckPointWidgetClass);
	if (CheckPointWidget)
	{
		CheckPointWidget->AddToViewport();
	}

	CinematicWidget = CreateWidget<UCinematicWidget>(GetWorld(), CinematicWidgetClass);
	if (CinematicWidget)
	{
		CinematicWidget->AddToViewport();
	}
}


UWidget* APlayerHUD::GetInventorySlot()
{
	return InGameWidget->InventoryMenuWidget;
}

UWidget* APlayerHUD::GetEquipmentSlot()
{
	return InGameWidget->EquipmentMenuWidget;
}

UWidget* APlayerHUD::GetSpellSlot()
{
	return CheckPointWidget->SpellWidget;
}

UWidget* APlayerHUD::GetStorageSlot()
{
	return CheckPointWidget->StorageWidget;
}

void APlayerHUD::ClearPopupWidget()
{
	if (PopupWidget)
	{
		PopupWidget->Teardown();
		PopupWidget = nullptr;
	}
	SetActiveMenu(EActiveMenu::PlayerMenu);
}

void APlayerHUD::TotalSwitchMenu(bool bForward)
{
	if(ActiveMenu == EActiveMenu::InGameMenu) InGameWidget->SwitchMenu(bForward);
	else if(ActiveMenu == EActiveMenu::CheckPointMenu) CheckPointWidget->SwitchMenu(bForward);
}

void APlayerHUD::TotalReverseMenu()
{

	//if(Owner->GetStartingStand()) return;
	if (ActiveMenu == EActiveMenu::InGameMenu)
	{
		InGameWidget->ReverseMenu();
	}
	if(ActiveMenu == EActiveMenu::CheckPointMenu)
	{
		CheckPointWidget->ReverseMenu();
	}
}

void APlayerHUD::TotalApplyMenu()
{
	if (ActiveMenu == EActiveMenu::InGameMenu)	
	{
		InGameWidget->ConfirmMenu();
	}
	else if(ActiveMenu == EActiveMenu::CheckPointMenu)
	{
		CheckPointWidget->ConfirmMenu();
	}
}

void APlayerHUD::OnUpdatedHealth()
{
	if(!PlayerWidget) return;

	PlayerWidget->GetHealthPercent();
}

void APlayerHUD::CallEventUI(EEventUIType EventType)
{
	if (PlayerWidget)
	{
		if (EventType == EEventUIType::FELLED)
		{
			PlayerWidget->PlayEventUI(FText::FromName("BOSS FELLED"), FELLEDTint);

			if (FELLEDSound) UGameplayStatics::PlaySound2D(GetWorld(), FELLEDSound);
		}
		else if (EventType == EEventUIType::DEAD)
		{
			PlayerWidget->PlayEventUI(FText::FromName("YOU DIED"), DEADTint);

			if(DEADSound) UGameplayStatics::PlaySound2D(GetWorld(), DEADSound);
		}
	}
}

void APlayerHUD::CallMessageUI(FName Message)
{
	if (PlayerWidget)
	{
		PlayerWidget->CallMessageBox(FText::FromName(Message));
	}
}

bool APlayerHUD::GetBossBoxActivate() const
{
	if (PlayerWidget && PlayerWidget->BossUI())
	{
		return PlayerWidget->BossUI()->GetVisibility() == ESlateVisibility::Hidden ? false : true;
	}
	return false;
}

bool APlayerHUD::GetAllSetupMenu() const
{
	if(InGameWidget && PlayerWidget) return true;

	return false;
}

void APlayerHUD::SetupEquipment()
{
	if (InGameWidget)
	{
		InGameWidget->ReadyEquipGradeBox();
	}
}

void APlayerHUD::ClearWidgetTimer()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(PlayerWidget);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(InGameWidget);
}

void APlayerHUD::SwitchCheckPointMenu()
{
	APlayerController* Con = GetWorld()->GetFirstPlayerController();
	if (!Con) return;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);

	Con->SetInputMode(InputModeData);
	Con->bShowMouseCursor = true;

	ActiveMenu = EActiveMenu::CheckPointMenu;
	CheckPointWidget->CallSelectMenu();
}



bool APlayerHUD::GetActivateMenu()
{
	return ActiveMenu != EActiveMenu::PlayerMenu;
}

void APlayerHUD::ShowPopupMenu(AActor* ParentActor, TSubclassOf<UUserWidget> PopupClass)
{
	if (PopupClass && ParentActor)
	{
		PopupWidget = CreateWidget<USmartObjectInteraction>(GetWorld(), PopupClass);
		if (PopupWidget)
		{
			bool bSuccess = PopupWidget->SetupData(ParentActor);
			bSuccess ? SetActiveMenu(EActiveMenu::Popup) : PopupWidget = nullptr;
			//PopupWidget->AddToViewport();
		}
	}
}

void APlayerHUD::SetPressedOnCinematicMode(bool bActive)
{
	if (CinematicWidget)
	{
		CinematicWidget->SetPercent(bActive);
	}
}

void APlayerHUD::EnableCinematicMode()
{
	APlayerController* Con = GetWorld()->GetFirstPlayerController();
	if (!Con) return;

	FInputModeGameOnly InputModeData;

	Con->SetInputMode(InputModeData);
	Con->bShowMouseCursor = false;

	ActiveMenu = EActiveMenu::Cinematic;

	CinematicWidget->StartCinematic();
	PlayerWidget->PlayWidgetAnim();
}

void APlayerHUD::DiableCinematicMode()
{
	APlayerController* Con = GetWorld()->GetFirstPlayerController();
	if (!Con) return;

	FInputModeGameOnly InputModeData;

	Con->SetInputMode(InputModeData);
	Con->bShowMouseCursor = false;

	ActiveMenu = EActiveMenu::PlayerMenu;
	PlayerWidget->PlayWidgetAnim();
}

void APlayerHUD::SetServerTimer(bool bActivate)
{
	if (PlayerWidget)
	{
		PlayerWidget->SetTimerVisibility(bActivate);
	}
}

void APlayerHUD::UpdateServerTimer(float InTimer)
{
	if (PlayerWidget)
	{
		PlayerWidget->UpdateTimer(InTimer);
	}
}

void APlayerHUD::PlayWarpMotion()
{
	if (WarpMt)
	{
		if (Owner->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) Owner->Server_InteractionMontage(WarpMt, NAME_None);

		APlayerController* Con = GetWorld()->GetFirstPlayerController();
		if (!Con) return;

		FInputModeGameOnly InputModeData;

		Con->SetInputMode(InputModeData);
		Con->bShowMouseCursor = false;

		ActiveMenu = EActiveMenu::PlayerMenu;
		PlayerWidget->PlayWidgetAnim();
	}
}


