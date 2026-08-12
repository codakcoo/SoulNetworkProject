// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "Interface/EventInterface.h"
#include "MovingObject.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AMovingObject : public ASmartObject, public IEventInterface
{
	GENERATED_BODY()

protected:
	AMovingObject();
	
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;


public:
	UFUNCTION(BlueprintCallable)
	void StartMoving();

	UFUNCTION()
	virtual void SetupObject(bool bCondition) override;
	//Interface//
	virtual void CallEvent() override;

protected:
	virtual void MovingEnd() PURE_VIRTUAL(AMovingObject, );

	UFUNCTION()
	void DirectionMovingFloat(float Value);
	UFUNCTION()
	void FinishedMoving();

private:
	void SetNiagaraValueAtMesh();

	UFUNCTION(Server, Reliable)
	void Server_SetNiagaraActivate(bool bActivate);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetNiagaraActivate(bool bActivate);

protected:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UNiagaraComponent* Niagara;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UAudioComponent* Audio;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	class UCurveFloat* MovingfCurve;

	FOnTimelineFloat MovingTimelineFloat;
	FOnTimelineEvent MovingEndEvent;

	FTimeline MovingTimeline;
private:
	
	FVector StartLocation;
	FVector EndLocation;

	//UPROPERTY(Replicated)
	//bool bMoving = false;
};
