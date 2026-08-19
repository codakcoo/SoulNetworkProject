// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/SignPointBlock.h"

#include "Interface/MenuInterface.h"

#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

ASignPointBlock::ASignPointBlock()
{
	PrimaryActorTick.bCanEverTick = true;

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	WallMesh->SetupAttachment(Billboard);
	WallMesh->SetIsReplicated(true);
	WallMesh->SetCollisionProfileName(TEXT("NoCollision"));

	SetReplicates(true);

	bOriginalCondition = false;
}

void ASignPointBlock::BeginPlay()
{
	Super::BeginPlay();

	//WallMesh->SetCollisionProfileName(bOriginalCondition ? TEXT("BlockAll") : TEXT("NoCollision"));
	//TimelineFade(bOriginalCondition ? 1.0f : 0.0f);
	RefreshObject();
	SoulDebug::ScreenLog(FString::Printf(TEXT("%s"), bOriginalCondition ? TEXT("True") : TEXT("False")), FColor::Blue, 10.f);
}

void ASignPointBlock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	OpacityTimeline.TickTimeline(DeltaSeconds);
}

void ASignPointBlock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASignPointBlock::SetSignPointWall(bool bActivate)
{
	bOriginalCondition = bActivate;

	WallMesh->SetCollisionProfileName(bOriginalCondition ? TEXT("BlockAll") : TEXT("NoCollision"));
	if (bOriginalCondition)
	{
		if (OpacityTimelineReverseCurve)
		{
			FOnTimelineFloat FadeInFunction;
			FadeInFunction.BindUFunction(this, FName("TimelineFade"));
			OpacityTimeline.AddInterpFloat(OpacityTimelineReverseCurve, FadeInFunction);
		}
	}
	else
	{
		if (OpacityTimelineCurve)
		{
			FOnTimelineFloat FadeoutFunction;
			FadeoutFunction.BindUFunction(this, FName("TimelineFade"));
			OpacityTimeline.AddInterpFloat(OpacityTimelineCurve, FadeoutFunction);
		}
	}
	OpacityTimeline.PlayFromStart();
}

void ASignPointBlock::RefreshObject()
{
	WallMesh->SetCollisionProfileName(bOriginalCondition ? TEXT("BlockAll") : TEXT("NoCollision"));
	TimelineFade(bOriginalCondition ? 1.0f : 0.0f);

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("%s"), bOriginalCondition ? TEXT("True") : TEXT("False")));
}

void ASignPointBlock::TimelineFade(float Value)
{
	UMaterialInterface* Material = WallMesh->GetMaterial(0);
	UMaterialInstanceDynamic* DynamicMaterial = WallMesh->CreateDynamicMaterialInstance(0, Material);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue("Fadeout", Value);
	}
}
