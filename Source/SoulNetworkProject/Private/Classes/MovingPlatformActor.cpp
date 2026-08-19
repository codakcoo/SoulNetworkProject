// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/MovingPlatformActor.h"

// Sets default values
AMovingPlatformActor::AMovingPlatformActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetMobility(EComponentMobility::Movable);
}

// Called when the game starts or when spawned
void AMovingPlatformActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
	State = EMovingState::Drive;
	GlobalTargetRotation = GetAxisAngle();
}

float AMovingPlatformActor::GetAxisAngle()
{
	return 0.f + (State == EMovingState::Drive ? Angle : 360.f - Angle);
}

// Called every frame
void AMovingPlatformActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FRotator Rotation = GetActorRotation();
		/*
		float JourneyLength = SelectAxis == EAxis::Pitch ? GetActorRotation().Pitch : GetActorRotation().Roll;
		if (State == EMovingState::Drive)
		{
			if(JourneyLength )
		}
		*/
		Rotation.Yaw += Rotation.Yaw * Speed * DeltaTime;
		SetActorRotation(Rotation);
	}
}

