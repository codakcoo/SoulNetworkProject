// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/PopupWidget.h"
#include "MousePopup.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UMousePopup : public UPopupWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:

	UFUNCTION()
		FText UpdatedCountText();

	UFUNCTION()
		int32 GetCurrnetCount() const { return CurrentCount; }
	UFUNCTION()
		void SetSlotData();

	UFUNCTION(BlueprintCallable)
		void SetPositionBySlot();

public:
	UPROPERTY(meta = (BindWidget))
		class UOverlay* PopupBox;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* OnAnimation;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
		class UWidgetAnimation* OffAnimation;

private:
	int32 CurrentCount = 1;

};
