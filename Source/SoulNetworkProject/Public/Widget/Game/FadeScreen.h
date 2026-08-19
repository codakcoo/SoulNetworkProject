// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/MenuWidget.h"
#include "FadeScreen.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UFadeScreen : public UMenuWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void PlayFadein();
	UFUNCTION(BlueprintCallable)
	void PlayFadeout();

protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Fadein;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Fadeout;
};
