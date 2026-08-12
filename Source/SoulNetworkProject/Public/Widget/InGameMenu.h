// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "Interface/SlateInterface.h"
#include "InGameMenu.generated.h"


UENUM(BlueprintType)
enum class EMenuType_InGame : uint8
{
	MainMenu		UMETA(DisplayName = "MainMenu"),
	EquipMenu		UMETA(DisplayName = "EquipMenu"),
	InventoryMenu	UMETA(DisplayName = "InventoryMenu"),
	StatusMenu		UMETA(DisplayName = "StatusMenu"),
	OptionMenu		UMETA(DisplayName = "OptionMenu")
};

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UInGameMenu : public UMenuWidget, public ISlateInterface
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	//virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//
	//virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:

	virtual void CallbackMenu() override { CallSelectMenu(); }
	
	////-------------Interface-------------//
	//void CallbackSwitcher();
	//virtual void CallbackSwitcher_Implementation() override;
	////----------------------------------//
	UFUNCTION()
	void CallSelectMenu();

	//Clicked//
	UFUNCTION()
	void OpenEquipment();
	UFUNCTION()
	void OpenInventory();
	UFUNCTION()
	void OpenStatus();
	UFUNCTION()
	void OpenOption();
	UFUNCTION()
	void CallMainMenu();
	UFUNCTION()
	void BGClicked();
	
	//Hoved//
	UFUNCTION()
		void EquipmentHovered() { FocusIndex = 0; UpdateFocus(); }
	UFUNCTION()
		void InventoryHovered() { FocusIndex = 1; UpdateFocus(); }
	UFUNCTION()
		void StatusHovered() { FocusIndex = 2; UpdateFocus(); }
	UFUNCTION()
		void OptionHovered() { FocusIndex = 3; UpdateFocus(); }
	UFUNCTION()
		void ExitHovered() { FocusIndex = 4; UpdateFocus(); }


	UFUNCTION()
	bool UpdateEnabledButton();

	UFUNCTION(BlueprintCallable)
	void ReverseMenu();
	UFUNCTION(BlueprintCallable)
	virtual void SwitchMenu(bool bForward);
	UFUNCTION(BlueprintCallable)
	void ConfirmMenu();

	UFUNCTION()
	void BindPawn(class APlayerCharacter* InPawn);

	UFUNCTION()
	ESlateVisibility UpdateVIsibility();

	UFUNCTION()
	void ReadyEquipGradeBox();


	UFUNCTION()
	virtual void ConfirmPopup() override;
	UFUNCTION()
	virtual void CancelPopup() override;
	UFUNCTION()
	virtual void BackgroundPopup() override;

	UFUNCTION()
	bool CanShowPopupWidget();

private:
	UFUNCTION()
	void UpdateFocus();

public:

	UPROPERTY(meta = (BindWidget))
	class UWidget* SelectWidget;
	UPROPERTY(meta = (BindWidget))
	class UOptionWidget* OptionMenuWidget;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentWidget* EquipmentMenuWidget;
	UPROPERTY(meta = (BindWidget))
	class UInvenType_InventorySlotWidget* InventoryMenuWidget;
	UPROPERTY(meta = (BindWidget))
	class UStatusMenuWidget* StatusMenuWidget;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UButton* EquipmentButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* InventoryButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* StatusButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* OptionButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* BGButton;

	UPROPERTY(meta = (BindWidget))
	class UWidget* EquipBG;
	UPROPERTY(meta = (BindWidget))
		class UWidget* InvenBG;
	UPROPERTY(meta = (BindWidget))
		class UWidget* StatusBG;
	UPROPERTY(meta = (BindWidget))
		class UWidget* OptionBG;
	UPROPERTY(meta = (BindWidget))
		class UWidget* ExitBG;

	UPROPERTY(EditAnywhere, Category = "Keyboard")
	FKey B_UpperKey;
	UPROPERTY(EditAnywhere, Category = "Keyboard")
	FKey B_LowerKey;
	UPROPERTY(EditAnywhere, Category = "Keyboard")
	FKey B_LeftKey;
	UPROPERTY(EditAnywhere, Category = "Keyboard")
	FKey B_RightKey;

	UPROPERTY(EditAnywhere, Category = "Gamepad")
	FKey P_UpperKey;
	UPROPERTY(EditAnywhere, Category = "Gamepad")
	FKey P_LowerKey;
	UPROPERTY(EditAnywhere, Category = "Gamepad")
	FKey P_LeftKey;
	UPROPERTY(EditAnywhere, Category = "Gamepad")
	FKey P_RightKey;


private:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Blink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* EquipBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* InvenBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* StatusBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OptionBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ExitBlink;

	EMenuType_InGame MenuType = EMenuType_InGame::MainMenu;

	class APlayerCharacter* Owner = nullptr;

	int8 FocusIndex = 0;
};
