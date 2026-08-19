// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "WaterArea.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AWaterArea : public ASmartObject
{
	GENERATED_BODY()
	
protected:
	AWaterArea();

	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_CallEventOverlapActor(class ACharacterBase* Chr);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* WaterMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* KillZone;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	class UNiagaraSystem* WaterSplashAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageType")
	TSubclassOf<class UDamageType> DivingDamageType;
};
