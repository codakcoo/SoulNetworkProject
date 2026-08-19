// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "PatrolPath.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API APatrolPath : public ASmartObject
{
	GENERATED_BODY()
	
public:
	APatrolPath();

	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Patrol")
	class USplineComponent* Path;

	void GetSplinePoint();

	TArray<FVector> Locations;
};
