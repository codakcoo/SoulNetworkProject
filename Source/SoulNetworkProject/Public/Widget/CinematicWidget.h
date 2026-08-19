// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CinematicWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UCinematicWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UCinematicWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION()
	void StartCinematic();
	UFUNCTION()
	void SetPercent(bool bActive);

	UFUNCTION()
	ESlateVisibility IsActiveInGameMenu();

private:
	UFUNCTION()
	void UpdatedPercent(float InDeltaTime);

	UFUNCTION()
	void FillPercent();
	UFUNCTION()
	void ClearPercent();

	UFUNCTION()
	void SetPercentInMaterial(float Value);

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* ProgressCircle;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* VisibleAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HiddenAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* BlinkAnim;

	bool bPressSpace = false;
	bool bPlay = false;

	float MaxSkip = 3.0f;
	float CurrentSkip = 0.0f;

	class APlayerCharacter* Owner;
};
