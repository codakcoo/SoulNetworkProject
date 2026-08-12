// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	void OnUpdatedHealthPercent(float MaxHealth, float CurrentHealth);

	void SetupNameText(FName PlayerName);

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNameText;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	class ACharacterBase* Owner;
};
