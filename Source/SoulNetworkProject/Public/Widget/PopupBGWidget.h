// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupBGWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UPopupBGWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	bool SetParentWidget(class UUserWidget* InParent);
	UFUNCTION()
	bool SetCurrSlot(class UUserWidget* InSlot);

protected:
	UFUNCTION()
	void BGOnClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* BGButton;

	class UUserWidget* ParentWidget = nullptr;
};
