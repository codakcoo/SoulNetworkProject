// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "Components/TimeLineComponent.h"
#include "SignPointBlock.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ASignPointBlock : public ASmartObject
{
	GENERATED_BODY()
	
protected:
	ASignPointBlock();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	void SetSignPointWall(bool bActivate);
	
	UFUNCTION()
	void RefreshObject();
	//UFUNCTION(NetMulticast, Reliable)
	//void Multicast_RefreshObject();

private:
	UFUNCTION()
	void TimelineFade(float Value);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UStaticMeshComponent* WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* OpacityTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* OpacityTimelineReverseCurve;

private:
	UPROPERTY()
	FTimeline OpacityTimeline;

};
