// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Setup();
	void Teardown();

	virtual void CallbackMenu() PURE_VIRTUAL(UMenuWidget, );
	virtual void SwitchMenu(bool bForward) PURE_VIRTUAL(UMenuWidget, ); 
};
