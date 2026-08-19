// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/PatrolPath.h"

#include "Components/SplineComponent.h"
#include "SoulNetworkDebug.h"

APatrolPath::APatrolPath()
{
	PrimaryActorTick.bCanEverTick = false;

	Path = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	Path->SetupAttachment(RootComponent);
	Path->bDrawDebug = true;	// 에디터 뷰포트용. 인게임 표시는 BeginPlay 에서 soul.DrawDebug 로 제어
}

void APatrolPath::BeginPlay()
{
	Super::BeginPlay();

	if (Path)
	{
		Path->SetHiddenInGame(!SoulDebug::IsDrawEnabled());
	}

	GetSplinePoint();
}

void APatrolPath::GetSplinePoint()
{
	for (int8 i = 0; i < Path->GetNumberOfSplinePoints(); i++)
	{
		Locations.Add(Path->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
	}
}
