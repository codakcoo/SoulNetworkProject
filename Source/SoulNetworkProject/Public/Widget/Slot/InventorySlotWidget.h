// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/StateWidgetInterface.h"
#include "SoulNetworkProject/ItemState.h"
#include "Classes/InventoryComponent.h"

#include "SoulNetworkProject/ItemData.h"


#include "InventorySlotWidget.generated.h"

/**
 * 
 */



UCLASS()
class SOULNETWORKPROJECT_API UInventorySlotWidget : public UUserWidget, public IStateWidgetInterface
{
	GENERATED_BODY()
	
public:
	UInventorySlotWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;


	//Interface//
	UFUNCTION()
	virtual void UpdateStateMenu(class USlotWidget* ItemSlot) override;
	/////////////


	void UpdatedWidget(class USlotWidget* ItemSlot);

	UFUNCTION()
	void ChangeInventorySlot(EItemType ItemState);

	UFUNCTION()
	bool UpdateEnabledMenu();

	UFUNCTION()
	void AlReadyGradeSlot();

protected:
	TArray<FInventoryData*> CallbackInventorySlot(EItemType SlotType, 
		uint8 CurrIndex, TArray<FInventoryData*> CurrCategoryData, 
		class UWrapBox* CurrGradeBox, class UGradeSlotWidget* CurrGradeWidget, 
		uint8 RowIndex, uint8 SlotIndex);

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameText_Inven;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameText_Item;
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage_Item;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WidgetText;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* GradeBox;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* InvenBlink;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ErrorBrake")
	FInventoryData dummy;

protected:
	class UInventoryComponent* InventoryComponent;
	EItemType WidgetItemType = EItemType::Expendable;

	TSubclassOf<class UUserWidget> GradeRowClass;

	int8 MenuIndex = 1;
};
