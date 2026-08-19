// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AIAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UAIAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UAIAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void SetCharacterProperties();

	UFUNCTION()
	void UpdatedTurn();

	UFUNCTION()
	void UpdateValue();

	UFUNCTION()
	void SetRootYawOffset();
	UFUNCTION()
	void SetPitch();

	UFUNCTION()
	void UpdateBowValue();

public:
	UPROPERTY(BlueprintReadOnly)
	class ACharacterBase* Character = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bAIMovement = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsAccelerating = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Speed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Direction = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float CrouchSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float LastInputDirection = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsFalling = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsInCombat = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bADS = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bStartTurn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bDodge = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bRoll = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bGuardUp = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bTwoHand = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bBow = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float TwoHandBlend = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float ShieldBlend = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animaation")
	float CrouchBlend = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animaation")
	float BowBlend = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float BowPitch = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float BowYaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Pitch = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Yaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float RootYawOffset = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float DistanceCurveValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float MaxTurnAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FName DistancePivot = "DistanceToPivot";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FName Turning  = "Turning";

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	//bool bSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bSitting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float YawActual = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		bool bLeft = false;
		/*

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float RootYawOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		FRotator HandRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float DistanceCurveValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float MaxTurnAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FName DistanceToPivot = TEXT("DistanceToPivot");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FName Turning = TEXT("Turning");
		*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	//bool 

protected:
	float YawLastTick = 0.f;
	float YawChangeOverFrame = 0.f;
	float DistanceCurveValueLastFrame = 0.f;
	float TurnDirection = 0.f;
	float ABSRootYawOffset = 0.f;
	float DistanceCurveDifference = 0.f;
	float YawToSubtract = 0.f;
	float YawMultiplier = 0.f;
};
