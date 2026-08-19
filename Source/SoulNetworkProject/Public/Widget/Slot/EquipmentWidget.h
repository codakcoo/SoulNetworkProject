// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Slot/InventorySlotWidget.h"
#include "Interface/EquipmentInterface.h"
#include "EquipmentWidget.generated.h"

/**
 * 
 */
typedef struct FOpenWidgetInfo
{
	int32 SlotIndex;
	EItemType ItemType;
	EEquipHand HandType;

	void SetInfo(int32 Index, EEquipHand InHandType, EItemType InItemType)
	{
		SlotIndex = Index;
		ItemType = InItemType;
		HandType = InHandType;
	}
	void ClearInfo()
	{
		SlotIndex = 0;
		ItemType = EItemType::All;
		HandType = EEquipHand::None;
	}
};


UCLASS()
class SOULNETWORKPROJECT_API UEquipmentWidget : public UInventorySlotWidget, public IEquipmentInterface
{
	GENERATED_BODY()
	
protected:
	UEquipmentWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void RefreshEquipment();
	UFUNCTION()
	void RefreshItemSlot();

	UFUNCTION()
	void AllSetSlotType();

	UFUNCTION()
	virtual void OnSelectMenu(EItemType ItemType) override;
	UFUNCTION()
	virtual void UpdateWidgetText(class UEquipmentType_SlotWidget* InSlot) override;
	UFUNCTION()
	virtual void InOutItemSlot(class USlotWidget* ItemSlot) override;

	UFUNCTION()
	void BackEquipMenu();
	UFUNCTION()
	bool IsActiveEquipMenu();

public:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* SlotSwitcher;

	//Equip Menu//
	UPROPERTY(meta = (BindWidget))
	class UWidget* EquipMenu;

	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* L_Weapon_Fir;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* L_Weapon_Sec;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* L_Weapon_Thi;


	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* R_Weapon_Fir;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* R_Weapon_Sec;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* R_Weapon_Thi;

	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* HelmetSlot;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* ArmorSlot;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* PantSlot;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* BootsSlot;

	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* Expendable_1;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* Expendable_2;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* Expendable_3;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* Expendable_4;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* Expendable_5;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* Expendable_6;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* Expendable_7;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* Expendable_8;

	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* FirstArrowSlot;
	UPROPERTY(meta = (BindWidget))
	class UEquipmentType_SlotWidget* SecondArrowSlot;
	////////////////


	//Select Menu//
	UPROPERTY(meta = (BindWidget))
	class UWidget* SelectMenu;

	//////////////

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* EquipBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* SelectBlink;

private:
	TArray<class UEquipmentType_SlotWidget*> ExpendableSlotArray;
	TArray<class UEquipmentType_SlotWidget*> L_WeaponSlotArray;
	TArray<class UEquipmentType_SlotWidget*> R_WeaponSlotArray;
	TArray<class UEquipmentType_SlotWidget*> ArrowSlotArray;

	FOpenWidgetInfo Info;
};
