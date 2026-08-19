// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/SmartObjectInteraction.h"
#include "TutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UTutorialWidget : public USmartObjectInteraction
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual bool SetupData(class AActor* InParent) override;

	UFUNCTION()
	void CancelClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* BGButton;
};
