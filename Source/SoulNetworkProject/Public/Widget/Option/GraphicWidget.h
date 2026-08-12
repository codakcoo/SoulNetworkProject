// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Containers/UnrealString.h"
#include "GraphicWidget.generated.h"


/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UGraphicWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION()
	void RollbackMenu();

	UFUNCTION()
	bool CheckOptionState();

private:

	UFUNCTION()
	void UpdateText();
	UFUNCTION()
	void ApplyClicked();

	UFUNCTION()
	void ResolutionDown();
	UFUNCTION()
	void ResolutionUp();
	UFUNCTION()
	void WindowModeDown();
	UFUNCTION()
	void WindowModeUp();
	UFUNCTION()
	void ShadowDown() {ShadowQuality > 0 ? ShadowQuality -=1 : ShadowQuality = 0; }
	UFUNCTION()
	void ShadowUp() {ShadowQuality < 4 ? ShadowQuality += 1 : ShadowQuality = 4; }
	UFUNCTION()
	void TextureDown() {TextureQuality > 0 ? TextureQuality -= 1 : TextureQuality = 0; }
	UFUNCTION()
	void TextureUp() {TextureQuality < 4 ? TextureQuality += 1 : TextureQuality = 4; }
	UFUNCTION()
	void FoliageDown() {FoliageQuality > 0 ? FoliageQuality -= 1 : FoliageQuality = 0; }
	UFUNCTION()
	void FoliageUp() {FoliageQuality < 4 ? FoliageQuality += 1 : FoliageQuality = 4; }
	UFUNCTION()
	void VSyncDown() { VSync = true; }
	UFUNCTION()
	void VSyncUp() { VSync = false; }
	UFUNCTION()
	void FrameRateDown() { FrameRate > 0 ? FrameRate -= 30 : FrameRate = 0; }
	UFUNCTION()
	void FrameRateUp() { FrameRate < 150 ? FrameRate += 30 : FrameRate = 150; }


	UFUNCTION()
  	FText GetEnumToText(const EWindowMode::Type EnumType);
	UFUNCTION()
	FText GetResolutionText(int8 Index);
	UFUNCTION()
	FText GetQualityText(int8 Quality);

	UFUNCTION()
	void SwitchingWindowMode(EWindowMode::Type Window);
	UFUNCTION()
	void SwithingResolution(int32 Index);
	UFUNCTION()
	void GetResolutionIndex(FIntPoint ResolutionData);

private:
	//해상도//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResolutionText;
	UPROPERTY(meta = (BindWidget))
	class UButton* ResolutionLeftButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ResolutionRightButton;
	//////////
	
	//윈도우//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WindowModeText;
	UPROPERTY(meta = (BindWidget))
	class UButton* WindowModeLeftButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* WindowModeRightButton;
	//////////

	//그림자//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ShadowText;
	UPROPERTY(meta = (BindWidget))
	class UButton* ShadowLeftButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ShadowRightButton;
	/////////

	//텍스쳐//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextureText;
	UPROPERTY(meta = (BindWidget))
	class UButton* TextureLeftButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* TextureRightButton;
	/////////

	//퀄리티//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FoliageText;
	UPROPERTY(meta = (BindWidget))
	class UButton* FoliageLeftButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* FoliageRightButton;
	/////////

	//V싱크//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* VSyncText;
	UPROPERTY(meta = (BindWidget))
	class UButton* VSyncLeftButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* VSyncRightButton;
	////////

	//프레임 속도//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FrameRateText;
	UPROPERTY(meta = (BindWidget))
	class UButton* FrameRateLeftButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* FrameRateRightButton;
	///////////////

	UPROPERTY(meta = (BindWidget))
	class UButton* ApplyButton;

	bool VSync;
	float FrameRate;
	int8 ShadowQuality;
	int8 FoliageQuality;
	int8 TextureQuality;
	int32 ResolutionIndex;
	FIntPoint Resolution;
	int32 WindowIndex;
	EWindowMode::Type WindowMode;
};
