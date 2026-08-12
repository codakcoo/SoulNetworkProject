// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Classes/InventoryComponent.h"

#include "SoulNetworkProject/ItemData.h"


#include "UseItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UUseItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//UFUNCTION()
	//void SwitchingUseItem(class AMasterPickupActor* UseItem, bool bWidgetAnim);
	UFUNCTION()
	void SwitchingUseItem(FInventoryData& UseItem, bool bWidgetAnim);


	//void AddSlotItem(class AMasterPickupActor* UseItem);
	//void RemoveSlotItem();

	void AddSlotItem(FInventoryData UseItem);
	void RemoveSlotItem();
	void PlaySlotAnim();
private:

	//void UpdateSlot();
	void UpdateSlot();


public:
	UPROPERTY(meta = (BindWidget))
	class UOverlay* ItemBox;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;
	UPROPERTY(meta = (BindWidget))
	class UBorder* CountBorder;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* BlinkSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundCue* BlinkSound;

private:
	//class AMasterPickupActor* Item;
	FInventoryData SlotData;
};
