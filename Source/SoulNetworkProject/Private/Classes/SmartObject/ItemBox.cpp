// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/ItemBox.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PickupActor.h"

#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AItemBox::AItemBox()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Billboard);
	Mesh->SetIsReplicated(true);

	TopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopMesh"));
	TopMesh->SetupAttachment(Billboard);
	TopMesh->SetIsReplicated(true);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(Billboard);
	Trigger->SetIsReplicated(true);

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(Billboard);

	MoveFloat.BindUFunction(this, TEXT("OpenTopValue"));
	MoveEnd.BindUFunction(this, TEXT("FinishedOpen"));
}

void AItemBox::BeginPlay()
{
	Super::BeginPlay();

	if (MovingfCurve)
	{
		TopMeshTimeline.AddInterpFloat(MovingfCurve, MoveFloat);
		TopMeshTimeline.SetTimelineFinishedFunc(MoveEnd);

		StartLoc = TopMesh->GetRelativeLocation();
		StartRot = TopMesh->GetRelativeRotation();

		if (!bOriginalCondition)
		{
			TopMesh->SetRelativeLocation(TargetLoc);
			TopMesh->SetRelativeRotation(TargetRot);

			Multicast_SetupCollision();
		}
	}
}

void AItemBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TopMeshTimeline.TickTimeline(DeltaSeconds);
}

void AItemBox::Interact(APlayerCharacter* Character)
{
	if (Character && bOriginalCondition && InteractionMt)
	{
		FVector LookLoc = GetActorLocation() + GetActorForwardVector() * 60.0f;

		FVector NewLoc = FVector(LookLoc.X, LookLoc.Y, Character->GetActorLocation().Z);
		FRotator NewRot = FRotator(0.0f, GetActorRotation().Yaw - 180.f, 0.0f);

		Character->Server_RunInteraction(this, NewLoc, NewRot);
		Character->Server_InteractionMontage(InteractionMt, NAME_None);
	}
}

FName AItemBox::GetInteractableName() const
{
	return TEXT("Open Item Box.");
}

void AItemBox::PlayInteractionMt()
{
	TopMeshTimeline.PlayFromStart();
	Multicast_SetupCollision();
}


void AItemBox::OpenTopValue(float value)
{
	TopMesh->SetRelativeLocation(FMath::Lerp(StartLoc, TargetLoc, value));
	TopMesh->SetRelativeRotation(FMath::Lerp(StartRot, TargetRot, value));
}

void AItemBox::FinishedOpen()
{
	SetOriginalCondition(false);
	APickupActor* PickupActor = GetWorld()->SpawnActor<APickupActor>(APickupActor::StaticClass(), GetActorTransform());
	if (PickupActor)
	{
		PickupActor->ContainContents(DataContents);
	}
}


void AItemBox::Multicast_SetupCollision_Implementation()
{
	SetShapeComponent(Trigger, TEXT("NoCollision"));
}
