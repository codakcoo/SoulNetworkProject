// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/PopupWidget.h"
#include "ActionMenuWidget.generated.h"

/**
 * 
 */

 UENUM(BlueprintType)
 enum class EActionType : uint8
 {
	None		UMETA(DisplayName = "None"),
	Drop		UMETA(DisplayName = "Drop"),
	Destroy		UMETA(DisplayName = "Destroy")
 };

UCLASS()
class SOULNETWORKPROJECT_API UActionMenuWidget : public UPopupWidget
{
	GENERATED_BODY()
	

protected:
	UFUNCTION()
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void UpdateActionMenu(class USlotWidget* InSlot = nullptr);

	void UpdatedCountWidget();

	UFUNCTION()
	void VisibilityActionMenu();
	UFUNCTION()
	void HiddenActionMenu();


	//ActionMenu Func//
	UFUNCTION()
	void UseClicked();
	UFUNCTION()
	void DropClicked();
	UFUNCTION()
	void DestroyClicked();
	UFUNCTION()
	void DropAtOnceClicked();
	UFUNCTION()
	void DestroyAtOnceClicked();
	////////////////////

	//CountMenu Func//
	UFUNCTION()
	FText UpdateExplanationText();

	//UFUNCTION()
	//void AcceptClicked();
	//UFUNCTION()
	//void CancelClicked();
	UFUNCTION()
	void IncreaseClicked();
	UFUNCTION()
	void DecreaseClicked();
	//////////////////

	UFUNCTION()
	int32 GetCurrentCount() const { return CurrentCount; }
	UFUNCTION()
	EActionType GetActionType() const { return ActionType; }

protected:
	UFUNCTION(BlueprintCallable)
	void SetPositionBySlot();

public:
	UPROPERTY(meta = (BindWidget))
	class UWidget* TotalBox;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	//Action Menu//
	UPROPERTY(meta = (BindWidget))
	class UOverlay* ActionMenu;
	UPROPERTY(meta = (BindWidget))
	class UButton* UseButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* DropButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* DestroyButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* DropAtOnceButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* DestroyAtOnceButton;
	//////////////

	//Count Menu//
	UPROPERTY(meta = (BindWidget))
	class UOverlay* CountMenu;			
	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* ExplanationText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountText;		
	UPROPERTY(meta = (BindWidget))		
	class UButton* IncreaseButton;		
	UPROPERTY(meta = (BindWidget))
	class UButton* DecreaseButton;		



	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OnAction;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OffAction;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* SwitchCountMenu;

private:
	FTimerHandle ActionHandle;

	int32 CurrentCount = 1;
	EActionType ActionType = EActionType::None;

	class USlotWidget* CurrentSlot;
};
