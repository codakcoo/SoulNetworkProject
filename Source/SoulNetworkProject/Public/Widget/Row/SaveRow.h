// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveRow.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API USaveRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;


public:
	UFUNCTION()
	void SetSaveData(const FText InText, uint8 InLevel, UTexture2D* InTexture);

	UFUNCTION()
	void SetParentWidget(class UUserWidget* InParent);

private:
	UFUNCTION()
	void SaveOnClicked();
	UFUNCTION()
	void DeleteClicked();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString SaveName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 SlotIndex;

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* UserImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LvText;

	UPROPERTY(meta = (BindWidget))
	class UButton* SaveButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* DeleteButton;

	class ULoadGameWidget* ParentWidget;
};
