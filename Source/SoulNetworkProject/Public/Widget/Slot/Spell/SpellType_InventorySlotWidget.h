// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Slot/InventorySlotWidget.h"
#include "Interface/SpellEquipmentInterface.h"
#include "SpellType_InventorySlotWidget.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class EMenuType_Spell : uint8
 {
	Equip		UMETA(DisplayName = "Equip"),
	Select		UMETA(DisplayName = "Select")
 };


UCLASS()
class SOULNETWORKPROJECT_API USpellType_InventorySlotWidget : public UInventorySlotWidget, public ISpellEquipmentInterface
{
	GENERATED_BODY()
	
protected:
	USpellType_InventorySlotWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void CallFirstMenu();

	UFUNCTION()
	void AllSetSlotType();

	//Dynamic Function//
	UFUNCTION()
	void RefreshSpellMenu();
	UFUNCTION()
	void RefreshItemSlot();
	///////////////////
	
	//Interface//
	UFUNCTION()
	virtual void UpdateEquipWidgetText(class USpellType_SlotWidget* InSlot) override;
	UFUNCTION()
	virtual void OnSelectMenu() override;

	UFUNCTION()
	virtual void UpdateInvenWidgetText(class USpellType_InvenSlotWidget* InSlot) override;
	UFUNCTION()
	virtual void InOutItemSlot(class USlotWidget* ItemSlot) override;
	////////////

	//Menu Active Function//
	UFUNCTION()
	void BackEquipSlot();
	UFUNCTION()
	void ActiveSelectSlot();

	UFUNCTION()
	bool CheckAll() { return SelectType == ESpellType::None ? false : true; }
	UFUNCTION()
	bool CheckAttackSorcery() { return SelectType == ESpellType::AttackSorcery ? false : true; }
	UFUNCTION()
	bool CheckAssistSorcery() { return SelectType == ESpellType::AssistSorcery ? false : true; }
	UFUNCTION()
	bool CheckAttackIncantation() { return SelectType == ESpellType::AttackIncantation ? false : true; }
	UFUNCTION()
	bool CheckAssistIncantation() { return SelectType == ESpellType::AssistIncantation ? false : true; }
	///////////////////////

	//OnClicked//
	UFUNCTION()
	void OnAllClicked();
	UFUNCTION()
	void OnAAttackSorceryClicked();
	UFUNCTION()
	void OnAssistSorceryClicked();
	UFUNCTION()
	void OnAttackIncantationClicked();
	UFUNCTION()
	void OnAssistIncantationClicked();

	UFUNCTION()
	void SwitchingMenu(bool bForward);
	UFUNCTION()
	bool IsActiveEquipMenu();

	UFUNCTION()
	EMenuType_Spell GetMenuType() { return MenuType; }

public:

	UPROPERTY(meta = (BindWidget))
	class UWidget* EquipSpellSlotsBox;

	UPROPERTY(meta = (BindWidget))
	class UWidget* SelectMenuBox;
	UPROPERTY(meta = (BindWidget))
	class UWidget* InvenMenuBox;


	////////////////
	UPROPERTY(meta = (BindWidget))
	class USpellType_SlotWidget* SpellSlot_Fir;
	UPROPERTY(meta = (BindWidget))
	class USpellType_SlotWidget* SpellSlot_Sec;
	UPROPERTY(meta = (BindWidget))
	class USpellType_SlotWidget* SpellSlot_Thi;
	UPROPERTY(meta = (BindWidget))
	class USpellType_SlotWidget* SpellSlot_For;


	//Inven MenuButton//
	UPROPERTY(meta = (BindWidget))
	class UButton* AllSpellButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* AttackSorceryButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* AssistSorceryButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* AttackIncantationButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* AssistIncantationButton;
	///////////////////

	//Select Menu//
	//UPROPERTY(meta = (BindWidget))
	//class UWidget* SelectMenu;

	//////////////

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Blink;

private:
	TArray<class USpellType_SlotWidget*> SpellSlotArray;


	EMenuType_Spell MenuType = EMenuType_Spell::Equip;
	ESpellType SelectType = ESpellType::None;

	TOptional<uint8> CurrentSlotIndex;
};
