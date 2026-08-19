// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/MenuWidget.h"
#include "Interface/SlateInterface.h"
#include "CheckPointMenu.generated.h"


UENUM(BlueprintType)
enum class EMenuType_CP : uint8
{
	MainMenu	UMETA(DisplayName = "MainMenu"),
	Warp		UMETA(DisplayName = "Warp"),
	Levelup		UMETA(DisplayName = "Levelup"),
	Potion		UMETA(DisplayName = "Potion"),
	Storage		UMETA(DisplayName = "Storage"),
	Spell		UMETA(DisplayName = "Spell")
};

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UCheckPointMenu : public UMenuWidget, public ISlateInterface
{
	GENERATED_BODY()


public:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:

	virtual void CallbackMenu() override { CallSelectMenu(); }

	////-------------Interface-------------//
	//void CallbackSwitcher();
	//virtual void CallbackSwitcher_Implementation() override;
	////----------------------------------//
	UFUNCTION()
	void CallSelectMenu();

	UFUNCTION()
	void OpenWarpMenu();
	UFUNCTION()
	void OpenLevelMenu();
	UFUNCTION()
	void OpenPotionMenu();
	UFUNCTION()
	void OpenStorageMenu();
	UFUNCTION()
	void OpenSpellMenu();
	UFUNCTION()
	void CloseMenu();

	UFUNCTION()
	void LevelHovered() { FocusIndex = 0; UpdateFocus(); }
	UFUNCTION()
	void PotionHovered() { FocusIndex = 1; UpdateFocus(); }
	UFUNCTION()
	void StorageHovered() { FocusIndex = 2; UpdateFocus(); }
	UFUNCTION()
	void SpellHovered() { FocusIndex = 3; UpdateFocus(); }
	UFUNCTION()
	void CloseHovered() { FocusIndex = 4; UpdateFocus(); }

	UFUNCTION()
	bool UpdateEnabledButton();

	UFUNCTION(BlueprintCallable)
	void ReverseMenu();
	UFUNCTION(BlueprintCallable)
	virtual void SwitchMenu(bool bForward);
	UFUNCTION(BlueprintCallable)
	void ConfirmMenu();

	//UFUNCTION()
	//	void PlaySelectAnim() { PlayAnimation(SelectBlink); }
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
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* SelectMenuWidget;
	UPROPERTY(meta = (BindWidget))
	class UWarpWidget* WarpWidget;
	UPROPERTY(meta = (BindWidget))
	class ULevelupWidget* LevelupWidget;
	UPROPERTY(meta = (BindWidget))
	class UPotionWidget* PotionWidget;
	UPROPERTY(meta = (BindWidget))
	class UCPInventorySlotWidget* StorageWidget;
	UPROPERTY(meta = (BindWidget))
	class USpellType_InventorySlotWidget* SpellWidget;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* WarpButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* LevelButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* PotionButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* StorageButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* SpellButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* BGButton;

	UPROPERTY(meta = (BindWidget))
	class UWidget* WarpBG;
	UPROPERTY(meta = (BindWidget))
	class UWidget* LevelBG;
	UPROPERTY(meta = (BindWidget))
	class UWidget* PotionBG;
	UPROPERTY(meta = (BindWidget))
	class UWidget* StorageBG;
	UPROPERTY(meta = (BindWidget))
	class UWidget* SpellBG;
	UPROPERTY(meta = (BindWidget))
	class UWidget* CloseBG;

private:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Blink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* LevelBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* PotionBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* StorageBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* SpellBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* CloseBlink;

	EMenuType_CP MenuType = EMenuType_CP::MainMenu;

	class APlayerCharacter* Owner = nullptr;
	int8 FocusIndex;
	
};
