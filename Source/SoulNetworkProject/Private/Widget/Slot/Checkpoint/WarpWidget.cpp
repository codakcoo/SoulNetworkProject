// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Checkpoint/WarpWidget.h"
#include "SaveGame/SaveCharacterState.h"
#include "Widget/UI/MapButtonWidget.h"
#include "Widget/UI/WarpRowWidget.h"
#include "Classes/PlayerCharacter.h"

#include "Interface/SaveInterface.h"
#include "Interface/SlateInterface.h"

#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"


bool UWarpWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if(CloseButton) CloseButton->OnClicked.AddDynamic(this, &UWarpWidget::CloseClicked);

	return true;
}

void UWarpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupMenu();
}

void UWarpWidget::UpdatedWidget(FString ApplyWorldName)
{
	CurrentWorldName = ApplyWorldName;
	UpdatedMapButton();
}

void UWarpWidget::UpdatedImage(UWidget* ApplyWidget)
{
	if (UWarpRowWidget* RowWidget = Cast<UWarpRowWidget>(ApplyWidget))
	{
		for (auto& Row : WarpRowBox->GetAllChildren())
		{
			if (Row == RowWidget)
			{
				WarpImage->SetBrushFromTexture(RowWidget->GetWarpImage());
				RowWidget->PlayBlinkAnimation(true);

				continue;
			}
			Cast<UWarpRowWidget>(Row)->PlayBlinkAnimation(false);
		}
	}
}

void UWarpWidget::WarpCheckPoint(FName WarpPointName)
{
	APlayerHUD* PH = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if(!PH || !GetGameInstance()) return;
	else if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		Cast<ISaveInterface>(GetGameInstance())->SaveRespawn(true);
		Cast<ISaveInterface>(GetGameInstance())->SaveLastCheckPoint(WarpPointName);

		PH->PlayWarpMotion();
	}
}

void UWarpWidget::UpdatedMapButton()
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		USaveCharacterState* CurrentSaveData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData();
		if (CurrentSaveData)
		{
			ButtonRowBox->ClearChildren();
			for (auto& ActiveMap : CurrentSaveData->ActivateMaps)
			{
				FName MapName = FName(*ActiveMap.WorldName);
				UTexture2D* MapImage = Cast<ISaveInterface>(GetGameInstance())->GetMapTexutre(MapName);
				if(!MapImage) return;

				UMapButtonWidget* ButtonWidget = CreateWidget<UMapButtonWidget>(this, ButtonRowClass);
				if (ButtonWidget)
				{
					ButtonRowBox->AddChild(ButtonWidget);
					ButtonWidget->AddToViewport();
					ButtonWidget->SetupData(this, MapImage, MapName);
					if(ActiveMap.WorldName == CurrentWorldName)
					{
						MapText->SetText(FText::FromName(MapName));
						ButtonWidget->SetIsEnabled(false);
					}
				}
			}
			UpdatedWarpRow(CurrentWorldName);
		}
	}
}

void UWarpWidget::UpdatedWarpRow(FString WorldName)
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		USaveCharacterState* CurrentSaveData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData();
		if (CurrentSaveData)
		{
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, CheckPoint.ObjectName);
			WarpRowBox->ClearChildren();
			FMapData CurrentMapData = CurrentSaveData->GetMapData(WorldName);
			for (auto& CheckPoint : CurrentMapData.CheckPoints)
			{
				UTexture2D* ApplyTexture = Cast<ISaveInterface>(GetGameInstance())->GetPointTexture(FName(*WorldName), FName(*CheckPoint.ObjectName));
				if(!ApplyTexture) continue;
				UWarpRowWidget* RowWidget = CreateWidget<UWarpRowWidget>(this, WarpRowClass);
				if (RowWidget)
				{
					WarpRowBox->AddChild(RowWidget);
					RowWidget->AddToViewport();
					RowWidget->SetupData(this, ApplyTexture, FName(*CheckPoint.ObjectName), !CheckPoint.Activates[0]);
					if(CurrentSaveData->SpawnData.LastCheckPointName.ToString() == CheckPoint.ObjectName) RowWidget->SetApplierPoint();
				}
			}
		}
		if(WarpRowBox->GetAllChildren().Num() > 0) UpdatedImage(WarpRowBox->GetChildAt(0));
	}
}

void UWarpWidget::SetupMenu()
{
	UpdatedWidget(Cast<ISaveInterface>(GetGameInstance())->GetCurrentWorlName());
}


void UWarpWidget::CloseClicked()
{
	//MenuSwitcher->SetActiveWidget(SelectMenu);
	if (ParentWidget->GetClass()->ImplementsInterface(USlateInterface::StaticClass()))
	{
		Cast<ISlateInterface>(ParentWidget)->CancelPopup();
		//PlayAnimation(Blink);
	}
}
