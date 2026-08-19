// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UMapButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

public:
	UFUNCTION()
	void SetupData(UUserWidget* InParentWidget, UTexture2D* InImage, FName InName);
	UFUNCTION()
	FName GetMapName() const { return MapName; }

protected:
	UFUNCTION()
	void MapButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* MapButton;
	UPROPERTY(meta = (BindWidget))
	class UImage* MapImage;

private:
	class UWarpWidget* ParentWidget;
	FName MapName = NAME_None;
};
