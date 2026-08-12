// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/SlateInterface.h"
#include "CharacterSelectorWidget.generated.h"

/**
 * 
 */
DECLARE_DELEGATE(FUpdatedSelect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatedChrselectorPopup, bool, bActive);

 UENUM(BlueprintType)
 enum class EChrSelect : uint8
 {
	None		UMETA(DisplayName = "None"),
	Warrior		UMETA(DisplayName = "Warrior"),
	Wizard		UMETA(DisplayName = "Wizard")
 };

UCLASS()
class SOULNETWORKPROJECT_API UCharacterSelectorWidget : public UUserWidget, public ISlateInterface
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;
	
public:
	//*******************Interface Function**********************//
	UFUNCTION()
	virtual void ConfirmPopup() override;
	UFUNCTION()
	virtual void CancelPopup() override;
	UFUNCTION()
	virtual void BackgroundPopup() override;
	//*****************************&*****************************//


protected:
	UFUNCTION()
	void WarriorClicked();
	UFUNCTION()
	void WizardClicked();

	UFUNCTION()
	void CreateClicked();

	UFUNCTION()
	void UpdateEditTextBox(const FText& InText);

	UFUNCTION()
	void ResetValue();
private:
	UFUNCTION()
	void OnUpdatedSelect();


public:
	UPROPERTY(BlueprintAssignable, Category = "Slot")
	FUpdatedChrselectorPopup UpdatedPopup;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Image")
	class UTexture2D* EnableImage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Image")
	class UTexture2D* DiaslbeImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Popup")
	TSubclassOf<class UPopupWidget> PopupClass;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* WarriorButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* WizardButton;

	UPROPERTY(meta = (BindWidget))
	class UBorder* WarriorBox;
	UPROPERTY(meta = (BindWidget))
	class UBorder* WizardBox;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* NameTextBox;
	UPROPERTY(meta = (BindWidget))
	class UButton* CreateButton;

	EChrSelect ChrSelect;

	FUpdatedSelect UpdatedSelect;

	UPopupWidget* ConfirmWidget = nullptr;
};
