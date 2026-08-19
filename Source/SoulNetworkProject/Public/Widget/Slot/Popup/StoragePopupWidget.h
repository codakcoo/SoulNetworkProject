// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/PopupWidget.h"
#include "SoulNetworkProject/ItemData.h"
#include "StoragePopupWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UStoragePopupWidget : public UPopupWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void CountUpCkicked();
	UFUNCTION()
	void CountDownCkicked();
	UFUNCTION()
	void CountUp10Ckicked();
	UFUNCTION()
	void CountDown10Ckicked();

	UFUNCTION()
	FText UpdatedCountText();

	UFUNCTION()
	int32 GetCurrnetCount() const { return CurrentCount; }
	UFUNCTION()
	void SetSlotData(class USlotWidget* InSlot);

	UFUNCTION(BlueprintCallable)
	void SetPositionBySlot();

public:
	//UPROPERTY(meta = (BindWidget))
	//class UButton* OneUpButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* UpButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* DownButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* Up10Button;
	UPROPERTY(meta = (BindWidget))
	class UButton* Down10Button;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountText;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* PopupBox;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OnAnimation;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OffAnimation;

private:
	int32 CurrentCount = 1;

	class USlotWidget* CurrentSlot;
};
