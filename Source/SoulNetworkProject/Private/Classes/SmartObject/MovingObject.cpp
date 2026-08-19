// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/MovingObject.h"

#include "Components/TimelineComponent.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"

AMovingObject::AMovingObject()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Billboard);
	Mesh->SetIsReplicated(true);

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(Billboard);
	Niagara->SetIsReplicated(true);

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(Billboard);

	MovingTimelineFloat.BindUFunction(this, TEXT("DirectionMovingFloat"));
	MovingEndEvent.BindUFunction(this, TEXT("FinishedMoving"));

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AMovingObject::BeginPlay()
{
	Super::BeginPlay();

	MovingTimeline.AddInterpFloat(MovingfCurve, MovingTimelineFloat);
	MovingTimeline.SetTimelineFinishedFunc(MovingEndEvent);

	StartLocation = GetActorLocation();
	EndLocation = GetActorLocation() + TargetLocation;

	Audio->Stop();

	Server_SetNiagaraActivate(false);
	SetActorLocation(bOriginalCondition ? StartLocation : EndLocation);
}

void AMovingObject::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MovingTimeline.TickTimeline(DeltaSeconds);
}


void AMovingObject::StartMoving()
{

}

void AMovingObject::SetupObject(bool bCondition)
{
	Super::SetupObject(bCondition);

	bOriginalCondition = bCondition;
}

void AMovingObject::CallEvent()
{
	bOriginalCondition = bOriginalCondition ? false : true;

	Server_SetNiagaraActivate(true);
	SetNiagaraValueAtMesh();
	Audio->Play();
	MovingTimeline.PlayFromStart();
}

void AMovingObject::DirectionMovingFloat(float value)
{
	SetActorLocation(bOriginalCondition ?
		FMath::Lerp(StartLocation, EndLocation, value) :
		FMath::Lerp(EndLocation, StartLocation, value));
}

void AMovingObject::FinishedMoving()
{
	if (Audio)
	{
		Audio->Stop();
		Server_SetNiagaraActivate(false);
	}
}

void AMovingObject::SetNiagaraValueAtMesh()
{
	if (Niagara)
	{
		Niagara->SetVariableObject("Mesh", Mesh->GetStaticMesh());
	}
}

void AMovingObject::Server_SetNiagaraActivate_Implementation(bool bActivate)
{
	Multicast_SetNiagaraActivate(bActivate);
}
void AMovingObject::Multicast_SetNiagaraActivate_Implementation(bool bActivate)
{
	bActivate ? Niagara->Activate() : Niagara->Deactivate();
}