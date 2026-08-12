// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "StartPoint.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AStartPoint : public ASmartObject
{
	GENERATED_BODY()
	
protected:
	AStartPoint();

	virtual void BeginPlay() override;

public:
	UFUNCTION()
	FVector GetRandomStartLocation() const;
	UFUNCTION()
	FVector GetIndexFromLocation(uint8 Index) const;

public:
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	TArray<FVector> StartLocations;
};
