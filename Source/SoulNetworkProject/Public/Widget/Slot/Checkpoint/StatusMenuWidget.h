// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UStatusMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UStatusMenuWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
};
