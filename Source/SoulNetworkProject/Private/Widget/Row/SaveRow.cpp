// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Row/SaveRow.h"
#include "SaveGame/ClientInfo.h"
#include "Interface/SaveInterface.h"
#include "Widget/UI/LoadGameWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

bool USaveRow::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	SaveButton->OnClicked.AddDynamic(this, &USaveRow::SaveOnClicked);
	DeleteButton->OnClicked.AddDynamic(this, &USaveRow::DeleteClicked);

	return true;
}

void USaveRow::SetSaveData(const FText InText, uint8 InLevel, UTexture2D* InTexture)
{
	if (InTexture)
	{
		SaveName = InText.ToString();
		PlayerText->SetText(FText::FromString(SaveName));
		LvText->SetText(FText::FromString(FString::FromInt(InLevel)));

		UserImage->SetBrushFromTexture(InTexture);
	}
}

void USaveRow::SetParentWidget(UUserWidget* InParent)
{
	ParentWidget = Cast<ULoadGameWidget>(InParent);
}

void USaveRow::SaveOnClicked()
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		//Cast<IMenuInterface>(GetGameInstance())->Host(SaveName);
		Cast<ISaveInterface>(GetGameInstance())->LoadTravel(SaveName);
	}
}

void USaveRow::DeleteClicked()
{
	//if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	//{
	//	Cast<ISaveInterface>(GetGameInstance())->RemoveSaveData(SaveName);
	//}
	if(ParentWidget) ParentWidget->SetPopup(SlotIndex);
}
