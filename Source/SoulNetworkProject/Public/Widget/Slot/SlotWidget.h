// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Classes/InventoryComponent.h"

#include "SoulNetworkProject/ItemData.h"


#include "SlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	//UFUNCTION()
	//void SetItemSlot(class AMasterPickupActor* InItem);
	UFUNCTION()
	void SetItemSlot(FInventoryData& DataRow);
	
	//UFUNCTION()
	//void RemoveSlot();
	UFUNCTION()
	void RemoveSlot();


	//UFUNCTION()
	//void UpdateSlot();
	UFUNCTION()
	void UpdateSlot();

	UFUNCTION()
	uint32 GetSlotIndex() { return SlotIndex; }
	UFUNCTION()
	void SetSlotIndex(uint32 Index) { SlotIndex = Index; }

	UFUNCTION()
	void SetSelect(bool bActive) { bSelected = bActive; }
	UFUNCTION()
	bool GetSelected() { return bSelected; }

	virtual void OnClicked() PURE_VIRTUAL(USlotWidget, );

	virtual void OnHovered();

public:
	UPROPERTY(meta = (BindWidget))
	class UOverlay* ItemBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage; //

	UPROPERTY(meta = (BindWidget))
	class UBorder* CountBorder; //

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountText;

	UPROPERTY(meta = (BindWidget))
	class UImage* EquipIcon; //

	//UPROPERTY()
	FInventoryData SlotData;
	//UPROPERTY()
	//TSharedPtr<FInventoryData> SlotData;
	class APlayerHUD* OwnerHUD;

protected:
	uint32 SlotIndex;

	UPROPERTY()
	bool bSelected = false;
};
