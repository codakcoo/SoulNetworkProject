// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/MenuWidget.h"
#include "SmartObjectInteraction.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API USmartObjectInteraction : public UMenuWidget
{
	GENERATED_BODY()
	
public:
	virtual bool SetupData(class AActor* ParentActor) PURE_VIRTUAL( USmartObjectInteraction, return false; );


protected:
	UPROPERTY()
	class AActor* Parent = nullptr;
};
