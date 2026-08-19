// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Checkpoint/LevelupWidget.h"
#include "Widget/PopupWidget.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/CharacterStateComponent.h"
#include "Interface/SaveInterface.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

ULevelupWidget::ULevelupWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool ULevelupWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if (UpButton)
	{
		UpButton->bIsEnabledDelegate.BindUFunction(this, TEXT("UpdateUpButtonEnabled"));
		UpButton->OnClicked.AddDynamic(this, &ULevelupWidget::UpButtonClicked);
	}
	if (DownButton)
	{
		DownButton->bIsEnabledDelegate.BindUFunction(this, TEXT("UpdateDownButtonEnabled"));
		DownButton->OnClicked.AddDynamic(this, &ULevelupWidget::DownButtonClicked);
	}
	if (ApplyButton)
	{
		ApplyButton->bIsEnabledDelegate.BindUFunction(this, TEXT("UpdateApplyButtonEnabled"));
		ApplyButton->OnClicked.AddDynamic(this, &ULevelupWidget::ApplyButtonClicked);
	}
	if (OwnSoulText) OwnSoulText->TextDelegate.BindUFunction(this, TEXT("UpdateOwnText"));
	if (RemainText) RemainText->TextDelegate.BindUFunction(this, TEXT("UpdateRemainText"));
	if (NeedText) NeedText->TextDelegate.BindUFunction(this, TEXT("UpdateNeedText"));
	if (CurrentText) CurrentText->TextDelegate.BindUFunction(this, TEXT("UpdateCurrentText"));
	if (NextText) NextText->TextDelegate.BindUFunction(this, TEXT("UpdateNextText"));

	return true;
}

void ULevelupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OwingPlayer = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (OwingPlayer)
	{
		MenuType = ELevelType::Levelup;

		DesiredCount = 0;
		RemainSoul = OwingPlayer->GetSoul();
		NeedSoul = OwingPlayer->GetStateComponent()->GetNextExp(OwingPlayer->GetLevel() + DesiredCount);
	}
}

void ULevelupWidget::SetupWidget()
{
	ClearPopupWidget();

	MenuType = ELevelType::Levelup;

	DesiredCount = 0;
	RemainSoul = OwingPlayer->GetSoul();
	NeedSoul = OwingPlayer->GetStateComponent()->GetNextExp(OwingPlayer->GetLevel() + DesiredCount);
	//PlayAnimation(Blink);
}

void ULevelupWidget::ClearPopupWidget()
{
	MenuType = ELevelType::Levelup;
	if (PopupWidget)
	{
		PopupWidget->AllRemoveWidget();
		PopupWidget = nullptr;
	}
}

void ULevelupWidget::UpButtonClicked()
{
	if (OwingPlayer)
	{
		DesiredCount += 1;
		NeedSoul = OwingPlayer->GetStateComponent()->GetNextExp(OwingPlayer->GetLevel() + DesiredCount);
		RemainSoul -= NeedSoul;
	}
}

void ULevelupWidget::DownButtonClicked()
{
	if (OwingPlayer)
	{
		DesiredCount -= 1;
		RemainSoul += NeedSoul;
		NeedSoul = OwingPlayer->GetStateComponent()->GetNextExp(OwingPlayer->GetLevel() + DesiredCount);
	}
}

void ULevelupWidget::ApplyButtonClicked()
{
	if (PopWidgetClass)
	{
		PopupWidget = CreateWidget<UPopupWidget>(this, PopWidgetClass);
		if (PopupWidget)
		{
			PopupWidget->SetupData(this);
			PopupWidget->SetDescriptorText(TEXT("Do you want to change to that state?"));

			MenuType = ELevelType::Popup;
		}
	}
}

bool ULevelupWidget::UpdateUpButtonEnabled()
{
	if(!OwingPlayer) return false;

	return RemainSoul >= NeedSoul ? true : false;
}

bool ULevelupWidget::UpdateDownButtonEnabled()
{
	if (!OwingPlayer) return false;

	return DesiredCount != 0 ? true : false;
}

bool ULevelupWidget::UpdateApplyButtonEnabled()
{
	if (!OwingPlayer) return false;

	return DesiredCount == 0 ? false : true;
}

FText ULevelupWidget::UpdateOwnText()
{
	FString OutputText = TEXT("-");
	if (OwingPlayer)
	{
		OutputText = FString::FromInt(OwingPlayer->GetSoul());
	}
	return FText::FromString(OutputText);
}

FText ULevelupWidget::UpdateRemainText()
{
	FString OutputText = TEXT("-");
	if (OwingPlayer)
	{
		//int32 remainSoul = RemainSoul;
		OutputText = FString::FromInt(RemainSoul);
	}
	return FText::FromString(OutputText);
}

FText ULevelupWidget::UpdateNeedText()
{
	FString OutputText = TEXT("-");
	if (OwingPlayer)
	{
		OutputText = FString::FromInt(NeedSoul);
	}
	return FText::FromString(OutputText);
}

FText ULevelupWidget::UpdateCurrentText()
{
	FString OutputText = TEXT("0");
	if (OwingPlayer)
	{
		OutputText = FString::FromInt(OwingPlayer->GetLevel());
	}
	return FText::FromString(OutputText);
}

FText ULevelupWidget::UpdateNextText()
{
	FString OutputText = TEXT("0");
	if (OwingPlayer)
	{
		OutputText = FString::FromInt(OwingPlayer->GetLevel() + DesiredCount);
	}
	return FText::FromString(OutputText);
}

void ULevelupWidget::ConfirmPopup()
{
	if (OwingPlayer && GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		OwingPlayer->SetLevel(OwingPlayer->GetLevel() + DesiredCount);
		OwingPlayer->AddSoul((OwingPlayer->GetSoul() - RemainSoul) * -1);

		Cast<ISaveInterface>(GetGameInstance())->AddSavePlayer(OwingPlayer);

		DesiredCount = 0;
		RemainSoul = 0;
		NeedSoul = OwingPlayer->GetStateComponent()->GetNextExp(OwingPlayer->GetLevel());

		ClearPopupWidget();
	}
}

void ULevelupWidget::CancelPopup()
{
	ClearPopupWidget();
}

void ULevelupWidget::BackgroundPopup()
{
	ClearPopupWidget();
}
