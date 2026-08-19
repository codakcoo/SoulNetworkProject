// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/SlateInterface.h"
#include "LoadGameWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatedLoadPopup, bool, bActive);

UCLASS()
class SOULNETWORKPROJECT_API ULoadGameWidget : public UUserWidget, public ISlateInterface
{
	GENERATED_BODY()
	
protected:
	ULoadGameWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void UpdatedSaveList();
	UFUNCTION()
	void CallMainMenu();

	UFUNCTION()
	void SetParentWidget(class UUserWidget* InParent);
	UFUNCTION()
	uint8 GetNumofSaveList() const;

	UFUNCTION()
	void SetPopup(uint8 Index);

	//*******************Interface Function**********************//
	UFUNCTION()
	virtual void ConfirmPopup() override;
	UFUNCTION()
	virtual void CancelPopup() override;
	UFUNCTION()
	virtual void BackgroundPopup() override;
	//*****************************&*****************************//

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Popup")
	TSubclassOf<class UPopupWidget> PopupClass;

	UPROPERTY(BlueprintAssignable, Category = "Slot")
	FUpdatedLoadPopup UpdatedPopup;

	UPROPERTY(meta = (BindWidget))
	class UButton* LoadGameBackButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LoadGameText;
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* LoadGameList;

	UPopupWidget* ConfirmWidget = nullptr;

private:
	TSubclassOf<class UUserWidget> SaveRowClass;
	class UMainMenu* ParentWidget;

	uint8 SelectDeleteSlotIndex = 0;
};
