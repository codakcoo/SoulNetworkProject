// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileComponent.generated.h"


UENUM(BlueprintType)
enum class ELoopType : uint8
{
	Projectile		UMETA(DisplayName = "Projectile"),
	Homing			UMETA(DisplayName = "Homing"),
	Track			UMETA(DisplayName = "Track")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULNETWORKPROJECT_API UProjectileComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	void RunProjectile(class ACharacterBase* InSpawnOwner, class ACharacterBase* InTrackChr = nullptr, FName InTrackBoneName = NAME_None);
	UFUNCTION(Server, Reliable)
	void Server_RunProjectile(class ACharacterBase* InSpawnOwner, class ACharacterBase* InTrackChr = nullptr, FName InTrackBoneName = NAME_None);

	UFUNCTION()
	void StopProjectile();

	UFUNCTION()
	void UpdatedProjectile(float InDeltaTime);
	UFUNCTION(Server, Reliable)
	void Server_UpdatedProjectile(float InDeltaTime);
private:
	UFUNCTION()
	FVector CalculateGravityAndDecelaration(FVector InVelocity, float InDeltaTime);
	UFUNCTION()
	float GetDragForce();
	UFUNCTION()
	float CalculateForwardPitch(FVector NextLocation, FVector CurrentLocation, FRotator CurrentRotation, float InDeltaTime);

	UFUNCTION()
	void DrawProjectileDebug();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	ELoopType LoopType = ELoopType::Projectile;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float Speed = 1000.0f;		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float AddRandSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "LoopType != ELoopType::Projectile"))
	float HomingSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "LoopType == ELoopType::Projectile"))
	bool bUseApplyRotation = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "LoopType == ELoopType::Projectile"))
	float AirDensity = 0.001225f;											 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "LoopType == ELoopType::Projectile"))
	float CrossSection = 1.04f;												   
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "LoopType == ELoopType::Projectile"))
	float DragCoeffciency = 0.0012f;									   
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "LoopType == ELoopType::Projectile"))
	float Gravity = 9.8f;															   
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "LoopType == ELoopType::Projectile"))
	float Mass = 0.015f;				

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	bool bDrawDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float MaxDrawTime = 15.0f;

protected:
	UPROPERTY(Replicated)
	bool bIsBulletInitialized = false;
	FVector Velocity = FVector::ZeroVector;
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(Replicated)
	class ACharacterBase* SpawnOwner = nullptr;
	UPROPERTY(Replicated)
	class ACharacterBase* TrackChr= nullptr;
	UPROPERTY(Replicated)
	FName TrackBoneName = NAME_None;
};
