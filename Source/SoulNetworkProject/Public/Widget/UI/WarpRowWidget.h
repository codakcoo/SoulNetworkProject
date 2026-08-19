// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarpRowWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UWarpRowWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

public:
	UFUNCTION()
	void SetupData(UUserWidget* InParentWidget, UTexture2D* InImage, FName InName, bool bAcitvate);
	UFUNCTION()
	FName GetRowName() const { return RowName; }
	UFUNCTION()
	UTexture2D* GetWarpImage() const { return WarpImage; }

	UFUNCTION()
	void PlayBlinkAnimation(bool bPlay);

	UFUNCTION()
	void SetApplierPoint();



protected:
	UFUNCTION()
	void RowButtonClicked();
	UFUNCTION()
	void RowButtonHovered();

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* RowButton;
	UPROPERTY(meta = (BindWidget))
	class UImage* BG;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarpNameText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ArrowImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Blink;

private:
	class UWarpWidget* ParentWidget;
	UTexture2D* WarpImage = nullptr;
	FName RowName = NAME_None;
};
