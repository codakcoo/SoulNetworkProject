// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Option/GraphicWidget.h"

#include "Engine/Engine.h"
#include "Math/IntPoint.h"
#include "GameFramework/GameUserSettings.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

bool UGraphicWidget::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;
	if(!ShadowLeftButton || !ShadowRightButton || 
	   !TextureLeftButton || !TextureRightButton || 
	   !WindowModeLeftButton || !WindowModeRightButton || 
	   !FoliageLeftButton || !FoliageRightButton ||
	   !VSyncLeftButton || !VSyncRightButton ||
	   !ResolutionLeftButton || !ResolutionRightButton ||
	   !FrameRateLeftButton || !FrameRateRightButton) return false;
		
	WindowModeLeftButton->OnClicked.AddDynamic(this, &UGraphicWidget::WindowModeDown);
	WindowModeRightButton->OnClicked.AddDynamic(this, &UGraphicWidget::WindowModeUp);
	
	ResolutionLeftButton->OnClicked.AddDynamic(this, &UGraphicWidget::ResolutionDown);
	ResolutionRightButton->OnClicked.AddDynamic(this, &UGraphicWidget::ResolutionUp);
	
	ShadowLeftButton->OnClicked.AddDynamic(this, &UGraphicWidget::ShadowDown);
	ShadowRightButton->OnClicked.AddDynamic(this, &UGraphicWidget::ShadowUp);
	
	TextureLeftButton->OnClicked.AddDynamic(this, &UGraphicWidget::TextureDown);
	TextureRightButton->OnClicked.AddDynamic(this, &UGraphicWidget::TextureUp);
	
	FoliageLeftButton->OnClicked.AddDynamic(this, &UGraphicWidget::FoliageDown);
	FoliageRightButton->OnClicked.AddDynamic(this, &UGraphicWidget::FoliageUp);

	VSyncLeftButton->OnClicked.AddDynamic(this, &UGraphicWidget::VSyncDown);
	VSyncRightButton->OnClicked.AddDynamic(this, &UGraphicWidget::VSyncUp);
	
	FrameRateLeftButton->OnClicked.AddDynamic(this, &UGraphicWidget::FrameRateDown);
	FrameRateRightButton->OnClicked.AddDynamic(this, &UGraphicWidget::FrameRateUp);

	ApplyButton->OnClicked.AddDynamic(this, &UGraphicWidget::ApplyClicked);

	return true;
}

void UGraphicWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RollbackMenu();
	//GEngine->GetGameUserSettings()->LoadSettings();
	//VSync = GEngine->GetGameUserSettings()->IsVSyncEnabled();
	//FrameRate = GEngine->GetGameUserSettings()->GetFrameRateLimit();
	//WindowMode = GEngine->GetGameUserSettings()->GetFullscreenMode();
	//FoliageQuality = GEngine->GetGameUserSettings()->GetFoliageQuality();
	//TextureQuality = GEngine->GetGameUserSettings()->GetTextureQuality();
	//ShadowQuality = GEngine->GetGameUserSettings()->GetShadowQuality();
	//Resolution = GEngine->GetGameUserSettings()->GetScreenResolution();
	//GetResolutionIndex(Resolution);
}

void UGraphicWidget::RollbackMenu()
{
	GEngine->GetGameUserSettings()->LoadSettings();
	VSync = GEngine->GetGameUserSettings()->IsVSyncEnabled();
	FrameRate = GEngine->GetGameUserSettings()->GetFrameRateLimit();
	WindowMode = GEngine->GetGameUserSettings()->GetFullscreenMode();
	FoliageQuality = GEngine->GetGameUserSettings()->GetFoliageQuality();
	TextureQuality = GEngine->GetGameUserSettings()->GetTextureQuality();
	ShadowQuality = GEngine->GetGameUserSettings()->GetShadowQuality();
	Resolution = GEngine->GetGameUserSettings()->GetScreenResolution();
	GetResolutionIndex(Resolution);
}

bool UGraphicWidget::CheckOptionState()
{
	GEngine->GetGameUserSettings()->LoadSettings();
	if(VSync != GEngine->GetGameUserSettings()->IsVSyncEnabled()) return false;
	if(FrameRate != GEngine->GetGameUserSettings()->GetFrameRateLimit()) return false;
	if(WindowMode != GEngine->GetGameUserSettings()->GetFullscreenMode()) return false;
	if(FoliageQuality != GEngine->GetGameUserSettings()->GetFoliageQuality()) return false;
	if(TextureQuality != GEngine->GetGameUserSettings()->GetTextureQuality()) return false;
	if(ShadowQuality != GEngine->GetGameUserSettings()->GetShadowQuality()) return false;
	if(Resolution != GEngine->GetGameUserSettings()->GetScreenResolution()) return false;

	return true;
}

void UGraphicWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateText();
}

void UGraphicWidget::ApplyClicked()
{
	GEngine->GetGameUserSettings()->SetScreenResolution(Resolution);
	GEngine->GetGameUserSettings()->SetShadingQuality(ShadowQuality);
	GEngine->GetGameUserSettings()->SetTextureQuality(TextureQuality);
	GEngine->GetGameUserSettings()->SetFoliageQuality(FoliageQuality);
	GEngine->GetGameUserSettings()->SetFullscreenMode(WindowMode);
	GEngine->GetGameUserSettings()->SetVSyncEnabled(VSync);
	GEngine->GetGameUserSettings()->SetFrameRateLimit(FrameRate);

	GEngine->GetGameUserSettings()->ApplySettings(false);
}

void UGraphicWidget::UpdateText()
{
	WindowModeText->SetText(GetEnumToText(WindowMode));

	ShadowText->SetText(GetQualityText(ShadowQuality));
	TextureText->SetText(GetQualityText(TextureQuality));
	FoliageText->SetText(GetQualityText(FoliageQuality));
	ResolutionText->SetText(GetResolutionText(ResolutionIndex));
	VSyncText->SetText(FText::FromString(VSync ? TEXT("On") : TEXT("Off")));
	FrameRateText->SetText(FText::FromString(FrameRate > 0.f ? FString::Printf(TEXT("%0.0f"), FrameRate) : TEXT("Auto")));
}

void UGraphicWidget::ResolutionDown()
{
	ResolutionIndex > 0 ? ResolutionIndex -= 1 : ResolutionIndex = 0;
	SwithingResolution(ResolutionIndex);
}

void UGraphicWidget::ResolutionUp()
{
	ResolutionIndex < 4 ? ResolutionIndex += 1 : ResolutionIndex = 4;
	SwithingResolution(ResolutionIndex);
}

void UGraphicWidget::WindowModeDown()
{
	if ((uint8)WindowMode > 0) WindowMode = static_cast<EWindowMode::Type>((uint8)WindowMode - 1);
	else WindowMode = static_cast<EWindowMode::Type>((uint8)EWindowMode::Fullscreen);
}

void UGraphicWidget::WindowModeUp()
{
	if((uint8)WindowMode < 2) WindowMode = static_cast<EWindowMode::Type>((uint8)WindowMode + 1);
	else WindowMode = static_cast<EWindowMode::Type>((uint8)EWindowMode::Windowed);
}

FText UGraphicWidget::GetEnumToText(const EWindowMode::Type EnumType)
{
	FText EnumText;
	EnumText = UEnum::GetDisplayValueAsText(EnumType);
	return EnumText;
}

FText UGraphicWidget::GetResolutionText(int8 Index)
{
	FText ResolutionQualityText;
	
	switch (Index)
	{
		case 0:
		{
			ResolutionQualityText = FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y));
		}
		return ResolutionQualityText;
		
		case 1:
		{
			ResolutionQualityText = FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y));
		}
		return ResolutionQualityText;
		
		case 2:
		{
			ResolutionQualityText = FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y));
		}
		return ResolutionQualityText; 

		case 3:
		{
			ResolutionQualityText = FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y));
		}
		return ResolutionQualityText;

		case 4:
		{
			ResolutionQualityText = FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y));
		}
		return ResolutionQualityText;

		default:
		{
			ResolutionQualityText = FText::FromString(TEXT("Disable"));
		}
		return ResolutionQualityText;
	}
}

FText UGraphicWidget::GetQualityText(int8 Quality)
{
	FText QualityText;
	switch (Quality)
	{
		case 0:
		{
			QualityText = FText::FromString(TEXT("Low"));
		}
		return QualityText;
		
		case 1:
		{
			QualityText = FText::FromString(TEXT("Medium"));
		}
		return QualityText;
		
		case 2:
		{
			QualityText = FText::FromString(TEXT("High"));
		}
		return QualityText;
		
		case 3:
		{
			QualityText = FText::FromString(TEXT("Epic"));
		}
		return QualityText;
		
		case 4:
		{
			QualityText = FText::FromString(TEXT("Cinematic"));
		}
		return QualityText;

		default:
		{
			QualityText = FText::FromString(TEXT("Disalbe"));
		}
		return QualityText;
	}
}

void UGraphicWidget::SwitchingWindowMode(EWindowMode::Type Window)
{
	switch (Window)
	{
		case EWindowMode::Fullscreen:
		{
			
		}
		break;
		case EWindowMode::WindowedFullscreen:
		break;
		case EWindowMode::Windowed:
		break;
		case EWindowMode::NumWindowModes:
		break;
		default:
		break;
	}
}

void UGraphicWidget::SwithingResolution(int32 Index)
{
	switch(Index)
	{
		case 0 : 
		{
			Resolution = FIntPoint(1280, 720);
		}
		break;
		case 1 :
		{
			Resolution = FIntPoint(1600, 900);
		}
		break;
		case 2 :
		{
			Resolution = FIntPoint(1920, 1080);
		}
		break;
		case 3 :
		{
			Resolution = FIntPoint(2560, 1440);
		}
		break;
		case 4 : 
		{
			Resolution = FIntPoint(3840, 2160);
		}
		break;

		default:
		break;
	}
}

void UGraphicWidget::GetResolutionIndex(FIntPoint ResolutionData)
{
	switch (ResolutionData.X)
	{
		case 1200 : ResolutionIndex = 0;
		break;
		case 1600 : ResolutionIndex = 1;
		break;
		case 1920 : ResolutionIndex = 2;
		break;
		case 2560 : ResolutionIndex = 3;
		break;
		case 3840 : ResolutionIndex = 4;
		break;
		default:
		break;
	}
}
