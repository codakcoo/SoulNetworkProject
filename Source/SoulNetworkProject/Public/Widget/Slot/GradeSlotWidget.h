// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoulNetworkProject/ItemState.h"
#include "SoulNetworkProject/ItemData.h"
#include "GradeSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UGradeSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UGradeSlotWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void SetupRowIndex(uint8 RowIndex);

	UFUNCTION()
	void UpdatedItemSlot(uint8 i, FInventoryData& Data);

	UFUNCTION()
	bool CanSameType(EItemType ItemState, uint8 DataType);
	UFUNCTION()
	bool CanStackData();
	UFUNCTION()
	uint8 SlotStackCount();

	UFUNCTION()
	void SetSlotEnabled(bool bActive, class USlotWidget* EnabledWidget = nullptr );
	UFUNCTION()
	class USlotWidget* GetSlotDisabled();

	UFUNCTION()
	void DiabledInheritedItem();
	UFUNCTION()
	void UpdatedSelectedItem(FInventoryData& SelectItem);//class USlotWidget* SelectSlot);

	UFUNCTION()
	TArray<class USlotWidget*> GetSlotArr() const { return SlotArray; }

protected:
	UPROPERTY(meta = (BindWidget))
	class USlotWidget* ItemSlot_0;
	UPROPERTY(meta = (BindWidget))
	class USlotWidget* ItemSlot_1;
	UPROPERTY(meta = (BindWidget))
	class USlotWidget* ItemSlot_2;
	UPROPERTY(meta = (BindWidget))
	class USlotWidget* ItemSlot_3;
	UPROPERTY(meta = (BindWidget))
	class USlotWidget* ItemSlot_4;

	UPROPERTY()
	TArray<class USlotWidget*> SlotArray; 

	uint8 Index;
};
