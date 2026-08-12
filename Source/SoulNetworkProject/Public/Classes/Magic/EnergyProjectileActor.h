// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Magic/MasterMagicActor.h"
#include "NiagaraComponent.h"
#include "EnergyProjectileActor.generated.h"

/**
 * 
 */


 UENUM(BlueprintType)
 enum class ECollisionType : uint8
 {
	 Spread		UMETA(DisplayName = "Spread"),
	 Form		UMETA(DisplayName = "Form")
 };

UCLASS()
class SOULNETWORKPROJECT_API AEnergyProjectileActor : public AMasterMagicActor
{
	GENERATED_BODY()
	
public:
	AEnergyProjectileActor();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	virtual void BeginData(class ACharacterBase* InOwner, class USkeletalMeshComponent* CastMesh) override;
	UFUNCTION()
	virtual void LoopData() override;
	UFUNCTION()
	virtual void MultipleData(class ACharacterBase* InOwner) override;

	UFUNCTION()
	virtual void EndData() override;

private:
	virtual void UpdatedCollisionEvent(float InDeltaTime) override;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_UpdatedCollisionEvent(float InDeltaTime);

	//UFUNCTION(NetMulticast, Reliable, WithValidation)
	//void Multicast_Reaction(FTransform SpawnTransform);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_RadialTrace(FTransform SpawnTransform);

	UFUNCTION()
	void UpdatedSpread(float InDeltaTime);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UProjectileComponent* ProjectileComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
	ECollisionType CollisionType = ECollisionType::Form;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "CollisionType == ECollisionType::Spread"))
	float MaxRangeRadius = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"), meta = (EditCondition = "CollisionType == ECollisionType::Spread"))
	float RangeInterpSpeed = 10.f;

	UPROPERTY(Replicated)
	class ACharacterBase* TrackChr;
	UPROPERTY(Replicated)
	FName TrackBoneName = NAME_None;

	UPROPERTY()
	float ColorScale = 1.f;

private:
	//UPROPERTY(Replicated)
	//bool bEndScale = false;
	UPROPERTY(Replicated)
	bool bStopMovement = false;

	//UPROPERTY(Replicated)
	//FVector Velocity;
	//UPROPERTY(Replicated)
	//FVector StartLocation;
};
