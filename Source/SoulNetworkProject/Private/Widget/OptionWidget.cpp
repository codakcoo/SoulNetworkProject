// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/OptionWidget.h"
#include "Widget/Option/GraphicWidget.h"
//#include "Widget/MenuWidget.h"
#include "Interface/MenuInterface.h"
#include "Widget/PopupWidget.h"

#include "Components/Button.h"
#include "Components/Widget.h"
#include "Components/Overlay.h"
#include "Components/WidgetSwitcher.h"

bool UOptionWidget::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	if(GraphicButton)
	{
		GraphicButton->OnClicked.AddDynamic(this, &UOptionWidget::EnterGraphic);
		GraphicButton->bIsEnabledDelegate.BindUFunction(this, "GetGraphicEnabled");
	}
	if(AudioButton)
	{
		AudioButton->OnClicked.AddDynamic(this, &UOptionWidget::EnterAudio);
		AudioButton->bIsEnabledDelegate.BindUFunction(this, "GetAudioEnabled");
	}
	if(GameplayButton)
	{
		GameplayButton->OnClicked.AddDynamic(this, &UOptionWidget::EnterGameplay);
		GameplayButton->bIsEnabledDelegate.BindUFunction(this, "GetGameplayEnabled");
	}
	//if(BackEnterButton) BackEnterButton->OnClicked.AddDynamic(this, &UOptionWidget::ExitOption);

	//if(YesButton) YesButton->OnClicked.AddDynamic(this, &UOptionWidget::EnterYesButton);
	//if(NoButton) NoButton->OnClicked.AddDynamic(this, &UOptionWidget::EnterNoButton);

	

	return true;
}

void UOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//ExitMenu->SetVisibility(ESlateVisibility::Hidden);
	UpdatedSectionButton();
}

void UOptionWidget::EnterGraphic()
{
	MenuIndex = 0;
	SectionType = ESectionType::GraphicSection;
	MenuSwitcher->SetActiveWidget(GraphicMenu);
	PlayAnimation(GraphicBlink);
	UpdatedSectionButton();
}

void UOptionWidget::EnterAudio()
{
	MenuIndex = 1;
	SectionType = ESectionType::AudioSection;
	MenuSwitcher->SetActiveWidget(AudioMenu);
	PlayAnimation(AudioBlink);
	UpdatedSectionButton();
}

void UOptionWidget::EnterGameplay()
{
	MenuIndex = 2;
	SectionType = ESectionType::GameplaySection;
	MenuSwitcher->SetActiveWidget(GameplayMenu);
	PlayAnimation(GameplayBlink);
	UpdatedSectionButton();
}

void UOptionWidget::ExitOption()
{
/*
	if(ParentWidget->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		IMenuInterface::Execute_CallbackSwitcher(ParentWidget);
	}
*/
	UGraphicWidget* Graphic = Cast<UGraphicWidget>(GraphicMenu);
	if (!Graphic) return;

	if (Graphic->CheckOptionState())
	{
		ParentWidget->CallbackMenu();
		return;
	}
	else
	{
		if (PopupClass)
		{
			ConfirmWidget = CreateWidget<UPopupWidget>(this, PopupClass);
			if (ConfirmWidget)
			{
				ConfirmWidget->SetupData(this);
				UpdatedPopup.Broadcast(true);
			}
		}

		//ExitMenu->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

//Option�޴� ����Ű//
void UOptionWidget::SwitchingMenu(bool bForward)
{
	if(bForward)
	{
		MenuIndex++;
		if(MenuIndex > 2) MenuIndex = 0;
	}
	else
	{
		MenuIndex--;
		if(MenuIndex < 0) MenuIndex = 2;
	}


	if(MenuIndex == 0) EnterGraphic();
	else if(MenuIndex == 1) EnterAudio();
	else if(MenuIndex == 2) EnterGameplay();
}

void UOptionWidget::ConfirmPopup()
{
	if (ConfirmWidget)
	{
		UpdatedPopup.Broadcast(false);
		ConfirmWidget->AllRemoveWidget();
		ConfirmWidget = nullptr;
		ParentWidget->CallbackMenu();
	}
}
void UOptionWidget::CancelPopup()
{
	if (ConfirmWidget)
	{
		UpdatedPopup.Broadcast(false);
		ConfirmWidget->AllRemoveWidget();
		ConfirmWidget = nullptr;
	}
}
void UOptionWidget::BackgroundPopup()
{
	CancelPopup();
}
//////////////////////

void UOptionWidget::EnterYesButton()
{
	//UGraphicWidget* Graphic = Cast<UGraphicWidget>(GraphicMenu);
	//if (!Graphic) return;
	//
	//Graphic->RollbackMenu();
	//ParentWidget->CallbackMenu();
	//ExitMenu->SetVisibility(ESlateVisibility::Hidden);
}

void UOptionWidget::EnterNoButton()
{
	//ExitMenu->SetVisibility(ESlateVisibility::Hidden);
}

void UOptionWidget::UpdatedSectionButton()
{
	GetGraphicEnabled();
	GetAudioEnabled();
	GetGameplayEnabled();
}