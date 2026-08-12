// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatformActor.generated.h"

UCLASS()
class SOULNETWORKPROJECT_API AMovingPlatformActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingPlatformActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float GetAxisAngle();

public:
	enum class ESelectAxis
	{
		Pitch,
		Roll
	};
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform")
	ESelectAxis SelectAxis;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform")
	float Angle = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform")
	float Speed;

private:

	enum class EMovingState
	{
		Drive,
		Reverse
	};

	EMovingState State;
	float GlobalTargetRotation;
};
