// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/StartPoint.h"

AStartPoint::AStartPoint()
{

}

void AStartPoint::BeginPlay()
{
	Super::BeginPlay();
}

FVector AStartPoint::GetRandomStartLocation() const
{
	uint8 RandomIndex = FMath::RandRange(0, StartLocations.Num() - 1);

	return GetIndexFromLocation(RandomIndex);
}

FVector AStartPoint::GetIndexFromLocation(uint8 Index) const
{
	if(StartLocations.Num() <= Index) return FVector::ZeroVector;

	return GetTransform().TransformPosition(StartLocations[Index]);
}
