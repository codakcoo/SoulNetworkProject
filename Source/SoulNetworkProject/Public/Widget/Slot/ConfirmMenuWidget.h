// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmMenuWidget.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API UConfirmMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

public:
	//UFUNCTION()
	//void ActiveConfirm(EConfirmType Confirm, EReactionType Reaction = EReactionType::Multi);
	UFUNCTION()
	void DisableConfirm();

	UFUNCTION()
	FText UpdateText();

	UFUNCTION()
	void AcceptClicked();
	UFUNCTION()
	void CancelClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ConfirmText;

	UPROPERTY(meta = (BindWidget))
	class UButton* AcceptButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OnConfirmMenu;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OffConfirmMenu;

private:
	FTimerHandle RemoveTimer;



	float Delay;
};
