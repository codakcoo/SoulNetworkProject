// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UPopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;


public:
	UFUNCTION()
	void SetupData(class UUserWidget* InParent);
	UFUNCTION()
	void SetDescriptorText(FString InText);

	UFUNCTION()
	void AllRemoveWidget();

private:
	UFUNCTION()
	void ConfirmClicked();
	UFUNCTION()
	void CancelClicked();

protected:
	UUserWidget* ParentWidget;

	UPROPERTY(EditAnywhere, Category = "BGClass")
	TSubclassOf<class UPopupBGWidget> BGClass;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DescriptorText;

	class UPopupBGWidget* BGWidget;
};
