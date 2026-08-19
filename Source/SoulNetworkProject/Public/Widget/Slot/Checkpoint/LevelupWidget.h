// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/SlateInterface.h"
#include "LevelupWidget.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class ELevelType : uint8
 {
		Levelup		UMETA(DisplayName = "Levelup"),
		Popup		UMETA(DisplayName = "Popup")
 };

UCLASS()
class SOULNETWORKPROJECT_API ULevelupWidget : public UUserWidget, public ISlateInterface
{
	GENERATED_BODY()

protected:
	ULevelupWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	ELevelType GetMenuType() const { return MenuType; }
	UFUNCTION()
	void SetupWidget();
	UFUNCTION()
	void ClearPopupWidget();

protected:
	UFUNCTION()
	void UpButtonClicked();
	UFUNCTION()
	void DownButtonClicked();
	UFUNCTION()
	void ApplyButtonClicked();

	UFUNCTION()
	bool UpdateUpButtonEnabled();
	UFUNCTION()
	bool UpdateDownButtonEnabled();
	UFUNCTION()
	bool UpdateApplyButtonEnabled();

	UFUNCTION()
	FText UpdateOwnText();
	UFUNCTION()
	FText UpdateRemainText();
	UFUNCTION()
	FText UpdateNeedText();

	UFUNCTION()
	FText UpdateCurrentText();
	UFUNCTION()
	FText UpdateNextText();

	//Interafec//
	UFUNCTION()
	virtual void ConfirmPopup() override;
	UFUNCTION()
	virtual void CancelPopup() override;
	UFUNCTION()
	virtual void BackgroundPopup() override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* UpButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* DownButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ApplyButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OwnSoulText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RemainText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NeedText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NextText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Blink;

	UPROPERTY(EditAnywhere, Category = "Color")
	FLinearColor IncreaseColor;
	UPROPERTY(EditAnywhere, Category = "Color")
	FLinearColor DecreaseColor;

	UPROPERTY(EditAnywhere, Category = "Popup")
	TSubclassOf<class UPopupWidget> PopWidgetClass;

private:
	uint32 RemainSoul = 0;
	uint32 DesiredCount = 0;
	uint32 NeedSoul = 0;

	ELevelType MenuType;

	class APlayerCharacter* OwingPlayer;
	class UPopupWidget* PopupWidget;
};
