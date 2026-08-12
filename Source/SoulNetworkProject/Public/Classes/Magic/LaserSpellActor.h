// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Magic/MasterMagicActor.h"
#include "LaserSpellActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ALaserSpellActor : public AMasterMagicActor
{
	GENERATED_BODY()
	
protected:
	ALaserSpellActor();

public:
	virtual void BeginData(class ACharacterBase* InOwner, class USkeletalMeshComponent* CastMesh) override;

	virtual void LoopData() override;

	virtual void EndData() override;
	
private:
	virtual void UpdatedCollisionEvent(float InDeltaTime) override;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_UpdatedBeamEnd(FVector InBeamLocation);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float LaserLength = 0.f;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
	float LengthSpeed = 0.f;

	float CurrentLaserLength = 0.f;

};
