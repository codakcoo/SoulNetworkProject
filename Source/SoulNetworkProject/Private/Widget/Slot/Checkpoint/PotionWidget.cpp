// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Checkpoint/PotionWidget.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/InventoryComponent.h"
#include "Interface/SlateInterface.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"


bool UPotionWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	//SwitchMenu//
	if (AddMenuButton) AddMenuButton->OnClicked.AddDynamic(this, &UPotionWidget::AddClicked);
	if (AllocateMenuButton) AllocateMenuButton->OnClicked.AddDynamic(this, &UPotionWidget::AllocateClicked);
	if (BackMenuButton) BackMenuButton->OnClicked.AddDynamic(this, &UPotionWidget::BackClicked);

	//Add//
	if(AddYesButton) AddYesButton->OnClicked.AddDynamic(this, &UPotionWidget::AddYesClicked);
	if(AddNoButton) AddNoButton->OnClicked.AddDynamic(this, &UPotionWidget::AddNoClicked);

	//Allocate//
	if(HealthButton) HealthButton->OnClicked.AddDynamic(this, &UPotionWidget::HealthClicked);
	if(ManaButton) ManaButton->OnClicked.AddDynamic(this, &UPotionWidget::ManaClicked);
	if(AllocateApplyButton)  AllocateApplyButton->OnClicked.AddDynamic(this, &UPotionWidget::ApplyClicked);

	//DoNot//
	if (DoNotOKButton) DoNotOKButton->OnClicked.AddDynamic(this, &UPotionWidget::CallSelectMenu);

	return true;
}

void UPotionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Owner = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (Owner)
	{
		MaxPotionCount = Owner->GetInventoryComponent()->GetMaxPotionCount();
		HealthCount = Owner->GetInventoryComponent()->GetMaxHealthCount();
		ManaCount = Owner->GetInventoryComponent()->GetMaxManaCount();

		if (MaxPotionCount == HealthCount + ManaCount)
		{
			HealthText->SetText(FText::FromString(FString::FromInt(HealthCount)));
			ManaText->SetText(FText::FromString(FString::FromInt(ManaCount)));
		}
	}
}

void UPotionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPotionWidget::CallSelectMenu()
{
	MenuSwitcher->SetActiveWidget(SelectMenu);
	MenuType = EPotionMenuType::Select;

	PlayAnimation(Blink);
}

void UPotionWidget::AddClicked()
{
	if(!Owner || !Owner->GetInventoryComponent()) return;

	if (Owner->GetInventoryComponent()->CanAddCountByPotions() && Owner->GetInventoryComponent()->GetPotionUpgradeItem())
	{
		MenuSwitcher->SetActiveWidget(AddCountMenu);
		MenuType = EPotionMenuType::Add;
	}
	else
	{
		MenuSwitcher->SetActiveWidget(DoNotMenu);
		MenuType = EPotionMenuType::DoNot;
	}
	PlayAnimation(Blink);
}

void UPotionWidget::AllocateClicked()
{
	MenuSwitcher->SetActiveWidget(AllocateMenu);
	MenuType = EPotionMenuType::Allocate;

	MaxPotionCount = Owner->GetInventoryComponent()->GetMaxPotionCount();
	HealthCount = Owner->GetInventoryComponent()->GetMaxHealthCount();
	ManaCount = Owner->GetInventoryComponent()->GetMaxManaCount();
	
	if (MaxPotionCount == HealthCount + ManaCount)
	{
		HealthText->SetText(FText::FromString(FString::FromInt(HealthCount)));
		ManaText->SetText(FText::FromString(FString::FromInt(ManaCount)));
	}
	PlayAnimation(Blink);

}

void UPotionWidget::BackClicked()
{
	MenuSwitcher->SetActiveWidget(SelectMenu);
	if (ParentWidget->GetClass()->ImplementsInterface(USlateInterface::StaticClass()))
	{
		Cast<ISlateInterface>(ParentWidget)->CancelPopup();
		PlayAnimation(Blink);
	}
}

void UPotionWidget::AddYesClicked()
{
	if(Owner && Owner->GetInventoryComponent())
	{
		MenuSwitcher->SetActiveWidget(SelectMenu);
		MenuType = EPotionMenuType::Select;

		if(Owner->GetInventoryComponent()->GetPotionUpgradeItem())
		{
			Owner->GetInventoryComponent()->AddMaxPotionCount();
		}
	}
}

void UPotionWidget::AddNoClicked()
{
	MenuSwitcher->SetActiveWidget(SelectMenu);
	MenuType = EPotionMenuType::Select;
}

void UPotionWidget::HealthClicked()
{
	if (!Owner || ManaCount == 0 || MaxPotionCount == HealthCount) return;

	if (MaxPotionCount == HealthCount + ManaCount)
	{
		HealthCount += 1;
		ManaCount -= 1;

		HealthText->SetText(FText::FromString(FString::FromInt(HealthCount)));
		ManaText->SetText(FText::FromString(FString::FromInt(ManaCount)));
	}
}

void UPotionWidget::ManaClicked()
{
	if(!Owner || HealthCount == 0 || MaxPotionCount == ManaCount) return;

	if (MaxPotionCount == HealthCount + ManaCount)
	{
		HealthCount -= 1;
		ManaCount += 1;

		HealthText->SetText(FText::FromString(FString::FromInt(HealthCount)));
		ManaText->SetText(FText::FromString(FString::FromInt(ManaCount)));
	}
}

void UPotionWidget::ApplyClicked()
{
	if (!Owner || !Owner->GetInventoryComponent()) return;

	Owner->GetInventoryComponent()->SetInGamePotionsCount(HealthCount, ManaCount);
	MenuSwitcher->SetActiveWidget(SelectMenu);
	MenuType = EPotionMenuType::Select;
}
