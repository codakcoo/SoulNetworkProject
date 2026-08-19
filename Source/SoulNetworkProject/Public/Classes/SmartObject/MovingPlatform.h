// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "Components/TimelineComponent.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AMovingPlatform : public ASmartObject
{
	GENERATED_BODY()
	
protected:
	AMovingPlatform();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick( float DeltaSeconds ) override;

protected:
	UFUNCTION()
	void DirectionMovingFloat(float value);
	UFUNCTION()
	void FinishedMovingFloat();

	UFUNCTION()
	void DownButtonFloat(float Value);
	UFUNCTION()
	void FinishedDownButton();

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	UFUNCTION()
	virtual void SetupObject(bool bCondition) override;

private:
	bool CanOnButton();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* Button;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* Trigger;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	//class UTimelineComponent* Timeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	class UCurveFloat* MovingfCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	class UCurveFloat* ButtonCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class UAudioComponent* Audio;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* ButtonCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* EndCue;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector TargetLocation;

private:
	FOnTimelineFloat PlatformFloat;
	FOnTimelineEvent PlatformEnd;

	FOnTimelineFloat ButtonFloat;
	FOnTimelineEvent ButtonEnd;

	FVector StartLocation;
	FVector EndLocation;

	FVector ButtonStartLocation;
	FVector ButtonEndLocation;

	FTimeline PlatformTimeline;
	FTimeline ButtonTimeline;

	UPROPERTY(Replicated)
	bool bMoving = false;
	UPROPERTY(Replicated)
	bool bReverseMoving = false;
};
