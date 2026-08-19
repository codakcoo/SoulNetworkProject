// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UI/LoadGameWidget.h"
#include "Widget/Row/SaveRow.h"
#include "Widget/MainMenu.h"
#include "Widget/PopupWidget.h"
#include "SaveGame/ClientInfo.h"

#include "Interface/MenuInterface.h"
#include "Interface/SaveInterface.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

ULoadGameWidget::ULoadGameWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> SaveRow_Class(TEXT("WidgetBlueprint'/Game/Widget/Game/SaveRow.SaveRow_C'"));
	if (SaveRow_Class.Succeeded()) SaveRowClass = SaveRow_Class.Class;
}


bool ULoadGameWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if (LoadGameBackButton) LoadGameBackButton->OnClicked.AddDynamic(this, &ULoadGameWidget::CallMainMenu);


	return false;
}

void ULoadGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void ULoadGameWidget::UpdatedSaveList()
{
	LoadGameList->ClearChildren();

	UClientInfo* ClientData = nullptr;
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		ClientData = Cast<IMenuInterface>(GetGameInstance())->GetClient();
		if (!ClientData) return ;

		int8 i = 0;
		for (FString& UserName : ClientData->SaveNames)
		{
			USaveRow* Row = CreateWidget<USaveRow>(this, SaveRowClass);
			if (!Row) return;

			//UMaterialInterface* MatInterface = Cast<IMenuInterface>(GetGameInstance())->SetupCharacterImage(FName(*Data), i);
			//if(!MatInterface) continue;

			UTexture2D* Texture = Cast<IMenuInterface>(GetGameInstance())->SetupCharacterImage(UserName, i);
			uint8 Level = Cast<IMenuInterface>(GetGameInstance())->GetUserLevel(UserName);
			Row->SetSaveData(FText::FromString(UserName), Level, Texture);
			Row->SetParentWidget(this);
			LoadGameList->AddChild(Row);
			i++;


		}
		LoadGameText->SetText(FText::FromString(FString::FromInt(i)));
		//bool bNewButtonEnabled = i == 5 ? false : true;
		//bool bLoadbuttonEnabled = i == 0 ? false : true;


		//NewGameButton->SetIsEnabled(i == 5 ? false : true);
		//LoadGameButton->SetIsEnabled(i == 0 ? false : true);
		//LoadGameText->SetText(FText::FromString(FString::FromInt(i)));
	}
	//return MakeTuple(true, true);
}

void ULoadGameWidget::CallMainMenu()
{
	if(ParentWidget) ParentWidget->CallMainMenu();
}

void ULoadGameWidget::SetParentWidget(UUserWidget* InParent)
{
	ParentWidget = Cast<UMainMenu>(InParent);
}

uint8 ULoadGameWidget::GetNumofSaveList() const
{
	if(!LoadGameList) return 0;
	
	return LoadGameList->GetChildrenCount() + 1;
}

void ULoadGameWidget::SetPopup(uint8 Index)
{
	if (PopupClass)
	{
		SelectDeleteSlotIndex = Index;
		ConfirmWidget = CreateWidget<UPopupWidget>(this, PopupClass);
		if (ConfirmWidget)
		{
			ConfirmWidget->SetupData(this);
			ConfirmWidget->SetDescriptorText(TEXT("Would you like to delete a Save Data?"));
			UpdatedPopup.Broadcast(true);
		}
	}
}

void ULoadGameWidget::ConfirmPopup()
{
	if (ConfirmWidget && GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()) && GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		UClientInfo* ClientData = Cast<IMenuInterface>(GetGameInstance())->GetClient();
		if (!ClientData) return;

		UpdatedPopup.Broadcast(false);
		ConfirmWidget->AllRemoveWidget();
		ConfirmWidget = nullptr;

		Cast<ISaveInterface>(GetGameInstance())->RemoveSaveData(ClientData->SaveNames[SelectDeleteSlotIndex]);
	}
}

void ULoadGameWidget::CancelPopup()
{
	if (ConfirmWidget)
	{
		UpdatedPopup.Broadcast(false);
		ConfirmWidget->AllRemoveWidget();
		ConfirmWidget = nullptr;
	}
}

void ULoadGameWidget::BackgroundPopup()
{
	CancelPopup();
}
