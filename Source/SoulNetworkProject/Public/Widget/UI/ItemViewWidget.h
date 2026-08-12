// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoulNetworkProject/ItemData.h"
#include "ItemViewWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UItemViewWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

public:
	void Setup(FInventoryData& Item);

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemNameText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* XSignText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ViewAnim;

};
