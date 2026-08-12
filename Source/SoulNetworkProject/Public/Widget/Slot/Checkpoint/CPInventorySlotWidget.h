// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Slot/InventorySlotWidget.h"
#include "Interface/StorageInventoryInterface.h"
#include "Interface/SlateInterface.h"
#include "CPInventorySlotWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EInventoryType_CP : uint8
{
	None		UMETA(DisplayName = "None"),
	Confirm		UMETA(DisplayName = "Confirm")
};


UCLASS()
class SOULNETWORKPROJECT_API UCPInventorySlotWidget : public UInventorySlotWidget, public IStorageInventoryInterface, public ISlateInterface
{
	GENERATED_BODY()
	



protected:
	UCPInventorySlotWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	//Interface//
	UFUNCTION()
	virtual void UpdatedItemSlot(class USlotWidget* ItemSlot) override;
	UFUNCTION()
	virtual void UpdateItemText(class USlotWidget* ItemSlot) override;

	UFUNCTION()
	virtual void ConfirmPopup() override;
	UFUNCTION()
	virtual void CancelPopup() override;
	UFUNCTION()
	virtual void BackgroundPopup() override;
	/////////////

	//OnClicked//
	UFUNCTION()
		void OnWeaponClicked();
	UFUNCTION()
		void OnHelmetClicked();
	UFUNCTION()
		void OnArmorClicked();
	UFUNCTION()
		void OnPantClicked();
	UFUNCTION()
		void OnBootsClicked();
	UFUNCTION()
		void OnExpendableClicked();
	UFUNCTION()
		void OnSpellClicked();
	UFUNCTION()
		void OnArrowClicked();
	UFUNCTION()
	void OnEnhancementClicked();
	////////////

	//Inventory Delegate//
	UFUNCTION()
		void RefreshInventory();
	////////////////

	UFUNCTION()
		void SwitchingMenu(bool bForward);


	FORCEINLINE bool GetActivateLeftMenu() const { return bLeft; }
	FORCEINLINE EInventoryType_CP GetInventoryType() const { return InventoryType; }

	UFUNCTION()
	void SetupContentMenu();

	UFUNCTION()
	void SwitchActiveMenu();
	 
private:
	//Text BindFunction//
	UFUNCTION()
		FText UpdateWidgetText();

	UFUNCTION()
		ESlateVisibility UpdateApplyBox();
	UFUNCTION()
		FText UpdateDescriptionText();
	/////////////////////




	//Enabled BindFunction//
	UFUNCTION()
		bool CheckWeapon() { return WidgetItemType == EItemType::Weapon ? false : true; }
	UFUNCTION()
		bool CheckHelmet() { return WidgetItemType == EItemType::Helmet ? false : true; }
	UFUNCTION()
		bool CheckArmor() { return WidgetItemType == EItemType::Armor ? false : true; }
	UFUNCTION()
		bool CheckPant() { return WidgetItemType == EItemType::Pant ? false : true; }
	UFUNCTION()
		bool CheckBoots() { return WidgetItemType == EItemType::Boots ? false : true; }
	UFUNCTION()
		bool CheckExpendable() { return WidgetItemType == EItemType::Expendable ? false : true; }
	UFUNCTION()
		bool CheckSpell() { return WidgetItemType == EItemType::Spell ? false : true; }
	UFUNCTION()
		bool CheckArrow() { return WidgetItemType == EItemType::Arrow ? false : true; }
	UFUNCTION()
	bool CheckEnhancement() { return WidgetItemType == EItemType::Enhancement ? false : true; }
	////////////////////////

	UFUNCTION()
	bool UpdateGradeBox() { return bLeft && InventoryType == EInventoryType_CP::None; }
	UFUNCTION()
	bool UpdateStorageGradeBox() { return !bLeft && InventoryType == EInventoryType_CP::None; }

	UFUNCTION()
	void CallApplyDetailData(FInventoryData InData);

public:
	UPROPERTY(meta = (BindWidget))
	class UWidget* StateSelectBox;

	//ItemType Button//
	UPROPERTY(meta = (BindWidget))
	class UButton* WeaponButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* HelmetButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ArmorButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* PantButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* BootsButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ExpendableButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* SpellButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ArrowButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* EnhancementButton;
	/////////////////////

	//Additional Widget//
	UPROPERTY(meta = (BindWidget))
	class UWidget* ApplyBox;
	///////////////////////

	//StorageInven//
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* StorageGradeBox;
	///////////////

	//Animation//
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Blink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* StorageInvenBlink;
	////////////


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PopupClass")
	TSubclassOf<class UUserWidget> ConfirmWidgetClass;
	UPROPERTY()
	class UStoragePopupWidget* ConfirmWidget;

private:
	class USlotWidget* SelectSlot;
	EInventoryType_CP InventoryType = EInventoryType_CP::None;

	bool bLeft = true;
};
