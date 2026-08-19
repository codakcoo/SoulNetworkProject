// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarpWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UWarpWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void UpdatedWidget(FString ApplyWorldName);
	UFUNCTION()
	void UpdatedImage(UWidget* ApplyWidget);
	UFUNCTION()
	void WarpCheckPoint(FName WarpPointName);

	UFUNCTION()
	void UpdatedMapButton();
	UFUNCTION()
	void UpdatedWarpRow(FString WorldName);

	UFUNCTION()
	void SetParentWidget(class UUserWidget* InParent) { ParentWidget = InParent; }

	UFUNCTION()
	void SetupMenu();

protected:
	UFUNCTION()
	void CloseClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ButtonRowBox;
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* WarpRowBox;
	UPROPERTY(meta = (BindWidget))
	class UImage* WarpImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MapText;
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Row")
	TSubclassOf<class UUserWidget> ButtonRowClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Row")
	TSubclassOf<class UUserWidget> WarpRowClass;

private:
	FString CurrentWorldName;
	UUserWidget* ParentWidget;
};
