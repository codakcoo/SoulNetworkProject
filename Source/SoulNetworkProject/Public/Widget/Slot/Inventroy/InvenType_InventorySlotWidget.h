// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Slot/InventorySlotWidget.h"
#include "Interface/InventoryInterface.h"
#include "Interface/SlateInterface.h"
#include "InvenType_InventorySlotWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EReactionType : uint8
{
	Multi		UMETA(DisplayName = "Multi"),
	Single		UMETA(DisplayName = "Single")
};

UENUM(BlueprintType)
enum class EConfirmType : uint8
{
	None		UMETA(DisplayName = "None"),
	Drop		UMETA(DisplayName = "Drop"),
	Destroy		UMETA(DisplayName = "Destroy")
};


 UENUM(BlueprintType)
 enum class EInventoryType : uint8 
 {
	None		UMETA(DisplayName = "None"),
	Action		UMETA(DisplayName = "Action"),
	Confirm		UMETA(DisplayName = "Confirm"),
	Seleced		UMETA(DisplayName = "Selected")
 };

UCLASS()
class SOULNETWORKPROJECT_API UInvenType_InventorySlotWidget : public UInventorySlotWidget, public IInventoryInterface, public ISlateInterface
{
	GENERATED_BODY()
	

protected:
	UInvenType_InventorySlotWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	//Interface//
	UFUNCTION()
	virtual void OnReaction(class USlotWidget* ItemSlot) override;
	UFUNCTION()
	virtual void OffReaction(class USlotWidget* ItemSlot) override;

	UFUNCTION()
	virtual void UpdatedWidgetText(class USlotWidget* ItemSlot) override;

	UFUNCTION()
	virtual void ActiveDropMenu() override;
	UFUNCTION()
	virtual void ActiveDestroyMenu() override;


	UFUNCTION()
	virtual void SetDropSelectMode() override;
	UFUNCTION()
	virtual void SetDestroySelectMode() override;

	UFUNCTION()
	virtual void UseReaction() override;

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



	UFUNCTION()
	void ActivateInventory();
	UFUNCTION()
	void ActivateAction();
	UFUNCTION()
	void ActivateConfirm(EConfirmType InConfirmType, EReactionType InReactionType);
	UFUNCTION()
	void ActivateSelected(EConfirmType InConfirmType, EReactionType InReactionType);

	FORCEINLINE EInventoryType GetInventoryType() const { return InventoryType; }
	FORCEINLINE EConfirmType GetConfirmType() const { return ConfirmType; }
	FORCEINLINE EReactionType GetReactionType() const { return ReactionType; }

private:
	//Text BindFunction//
	UFUNCTION()
	FText UpdateWidgetText();

	UFUNCTION()
	ESlateVisibility UpdateApplyBox();
	UFUNCTION()
	FText UpdateDescriptionText();
	/////////////////////

	UFUNCTION()
	void AllCheck();

	UFUNCTION()
	void UpdatedItemSlots(bool bEnabledOtherSlot, class USlotWidget* ItemSlot = nullptr);
	UFUNCTION()
	void DiabledInheritedItemSlots();
	UFUNCTION()
	void UpdatedSelectedItemSlots();
	UFUNCTION()
	void CheckStackCount();
	

	//Enabled BindFunction//
	UFUNCTION()
	bool CheckStateBox() { return InventoryType == EInventoryType::Action ? false : true; }

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

public:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* StateSelectBox;	
	
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
	class UTextBlock* DescriptionText;
	UPROPERTY(meta = (BindWidget))
	class UWidget* ApplyBox;
	///////////////////////


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Additional")
		TSubclassOf<UUserWidget> ActionWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Additional")
		TSubclassOf<UUserWidget> ConfirmWidgetClass;

protected:
	class UActionMenuWidget* ActionWidget = nullptr;
	class UPopupWidget* ConfirmWidget = nullptr;

private:
	TArray<class USlotWidget*> SelectSlots;

	EInventoryType InventoryType = EInventoryType::None;

	EConfirmType ConfirmType = EConfirmType::None;
	EReactionType ReactionType = EReactionType::Single;

	EItemType BeforeType = EItemType::All;
};
