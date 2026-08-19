// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UI/CharacterSelectorWidget.h"
#include "Widget/PopupWidget.h"
#include "Interface/SaveInterface.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"

bool UCharacterSelectorWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;


	if(WarriorButton) WarriorButton->OnClicked.AddDynamic(this, &UCharacterSelectorWidget::WarriorClicked);
	if(WizardBox) WizardButton->OnClicked.AddDynamic(this, &UCharacterSelectorWidget::WizardClicked);
	if(CreateButton) CreateButton->OnClicked.AddDynamic(this, &UCharacterSelectorWidget::CreateClicked);
	if(NameTextBox) NameTextBox->OnTextChanged.AddDynamic(this, &UCharacterSelectorWidget::UpdateEditTextBox);
	//if(WarriorBox)	WarriorBox->OnMouseButtonDownEvent.BindUFunction(this, TEXT("WarriorClicked"));
	//if(WizardBox)	WarriorBox->OnMouseButtonDownEvent.BindUFunction(this, TEXT("WizardClicked"));
	//if(NameTextBox)
	//if(CreateButton)

	UpdatedSelect.BindUFunction(this, TEXT("OnUpdatedSelect"));

	return true;
}

void UCharacterSelectorWidget::ConfirmPopup()
{
	if (ConfirmWidget && GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		UpdatedPopup.Broadcast(false);
		ConfirmWidget->AllRemoveWidget();
		ConfirmWidget = nullptr;
		Cast<ISaveInterface>(GetGameInstance())->NewTravel(NameTextBox->GetText().ToString(), (uint8)ChrSelect);
	}
}

void UCharacterSelectorWidget::CancelPopup()
{
	if (ConfirmWidget)
	{
		UpdatedPopup.Broadcast(false);
		ConfirmWidget->AllRemoveWidget();
		ConfirmWidget = nullptr;
	}
}

void UCharacterSelectorWidget::BackgroundPopup()
{
	CancelPopup();
}

void UCharacterSelectorWidget::WarriorClicked()
{
	ChrSelect == EChrSelect::Warrior ? ChrSelect = EChrSelect::None : ChrSelect = EChrSelect::Warrior;

	UpdatedSelect.Execute();
}

void UCharacterSelectorWidget::WizardClicked()
{
	ChrSelect == EChrSelect::Wizard ? ChrSelect = EChrSelect::None : ChrSelect = EChrSelect::Wizard;
	UpdatedSelect.Execute();
}

void UCharacterSelectorWidget::CreateClicked()
{
	if (PopupClass)
	{
		ConfirmWidget = CreateWidget<UPopupWidget>(this, PopupClass);
		if (ConfirmWidget)
		{
			ConfirmWidget->SetupData(this);
			ConfirmWidget->SetDescriptorText(TEXT("Would you like to create a character?"));
			UpdatedPopup.Broadcast(true);
		}
	}
}

void UCharacterSelectorWidget::UpdateEditTextBox(const FText& InText)
{
	FString ForLength = InText.ToString();


	if (ForLength.Len() > 3 &&
		!UGameplayStatics::DoesSaveGameExist(ForLength, 0) &&
		ChrSelect != EChrSelect::None)
	{
		CreateButton->SetIsEnabled(true);
	}
	else
	{
		CreateButton->SetIsEnabled(false);
	}
}

void UCharacterSelectorWidget::ResetValue()
{
	ChrSelect = EChrSelect::None;
	NameTextBox->SetText(FText());

	UpdatedSelect.Execute();
}

void UCharacterSelectorWidget::OnUpdatedSelect()
{
	WarriorBox->SetBrushFromTexture(ChrSelect == EChrSelect::Warrior ? EnableImage : DiaslbeImage);
	WizardBox->SetBrushFromTexture(ChrSelect == EChrSelect::Wizard ? EnableImage : DiaslbeImage);


	NameTextBox->OnTextChanged.Broadcast(NameTextBox->GetText());
}
