// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/AIAnimInstance.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UAIAnimInstance::UAIAnimInstance()
{

}

void UAIAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<ACharacterBase>(TryGetPawnOwner());

	if(Cast<APlayerCharacter>(Character))
	{
		bAIMovement = false;
	}
	else
	{
		bAIMovement = true;
	}
}

void UAIAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!Character || DeltaSeconds == 0.f) return ;

	UpdateValue();
	SetCharacterProperties();
	UpdatedTurn();

	ShieldBlend = UKismetMathLibrary::FInterpTo(ShieldBlend, bGuardUp ? 1.f : 0.f, DeltaSeconds, 10.f);
	CrouchBlend = UKismetMathLibrary::FInterpTo(CrouchBlend, bIsCrouching ? 1.f : 0.f, DeltaSeconds, 10.f);
	TwoHandBlend = UKismetMathLibrary::FInterpTo(TwoHandBlend, bTwoHand? 1.f : 0.f, DeltaSeconds, 10.f);
	BowBlend = UKismetMathLibrary::FInterpTo(BowBlend, bBow ? 1.f : 0.f, DeltaSeconds, 10.f);


	if (bBow) UpdateBowValue();
	if (bIsInCombat)
	{
		SetPitch();
		return;
	}

	RootYawOffset = 0.f;
}

void UAIAnimInstance::SetCharacterProperties()
{
	bIsFalling = Character->GetCharacterMovement()->IsFalling();
	bIsCrouching = Character->GetCharacterMovement()->IsCrouching();
	bRoll = Character->AnimValues.bRoll;
	bTwoHand = Character->GetHoldType() != EEquipHand::None ? true : false;//Character->AnimValues.bTwoHand;
	bBow = Character->AnimValues.bAim;
	bADS = Character->AnimValues.bADS;

	if (Character->GetCombatComponent())
	{
		bGuardUp = Character->GetCombatComponent()->GetGuardup();
	}
	bIsInCombat = Character->AnimValues.bCombat;
	//bIsShooting = Character->AnimValues.bIsShooting;
	//bSitting = Character->AnimValues.bIsSitting;
	//YawActual = RootYawOffset * -1.f;
	//bSprinting = SetSprinting();
}

void UAIAnimInstance::UpdatedTurn()
{
	//float TargetYaw = Character->GetFocusRotator().Yaw - Character->GetActorRotation().Yaw;
	float TargetYaw = UKismetMathLibrary::NormalizedDeltaRotator(Character->GetActorRotation(), Character->GetFocusRotator()).Yaw ;
	YawActual = TargetYaw * -1.0f;
	if (bADS && Speed == 0.0f)
	{
		if(((TargetYaw >= 0.0f && TargetYaw < MaxTurnAngle) || (TargetYaw < 0.0f && TargetYaw > -MaxTurnAngle)))
		{
			bStartTurn = false;
			return;
		}

		bStartTurn = true;
		bLeft = TargetYaw > 0.0f ? true : false;

		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Red, FString::Printf(TEXT("%f"), TargetYaw));
		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green, bLeft ? TEXT("True") : TEXT("False"));
		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Blue, bRight ? TEXT("True") : TEXT("False"));

		return;
	}

	YawActual = 0.0f;
	bStartTurn = false;
}

void UAIAnimInstance::UpdateValue()
{
	bIsAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;

	FVector NewVelocity = Character->GetVelocity();
	Speed = NewVelocity.Size();
	//CrouchSpeed = 
	Direction = CalculateDirection(NewVelocity, Character->GetActorRotation());
	//Character->IsPlayingMontage() ? 0.f : 

	if (bIsAccelerating)
	{
		LastInputDirection = Direction;
	}
	//Multicast_UpdateValue();
}

void UAIAnimInstance::SetRootYawOffset()
{
	if (Speed > 0.f || IsAnyMontagePlaying())
	{
		RootYawOffset = 0.f;
	}

	YawLastTick = Yaw;
	Yaw = Character->GetActorRotation().Yaw;
	YawChangeOverFrame = YawLastTick - Yaw;

	RootYawOffset = UKismetMathLibrary::NormalizeAxis(YawChangeOverFrame + RootYawOffset);

	if (GetCurveValue(Turning) > 0.f)
	{
		DistanceCurveValueLastFrame = DistanceCurveValue;
		DistanceCurveValue = GetCurveValue(DistancePivot);

		(RootYawOffset > 0.f) ? TurnDirection = -1.f : TurnDirection = 1.f;
		DistanceCurveDifference = DistanceCurveValueLastFrame - DistanceCurveValue;
		ABSRootYawOffset = UKismetMathLibrary::Abs(RootYawOffset);
		if (ABSRootYawOffset > MaxTurnAngle)
		{
			YawToSubtract = ABSRootYawOffset - MaxTurnAngle;
			YawMultiplier = 0.f;
			(RootYawOffset > 0.f) ? YawMultiplier = 1.f : YawMultiplier = -1.f;
			YawToSubtract = YawToSubtract * YawMultiplier;

			RootYawOffset = RootYawOffset - YawToSubtract;
		}
	}

	return;
}

void UAIAnimInstance::SetPitch()
{
	//FRotator PawnRotation = Character->GetActorRotation();
	//FRotator AimRotation = Character->GetBaseAimRotation();
	//Pitch = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, PawnRotation).Pitch;
	if(!Character->GetFocusActor()) Pitch = 0.0f;
	else
	{
		FVector PawnRotation = Character->GetMesh()->GetSocketLocation(TEXT("head"));
		FVector AimRotation = Character->GetFocusActor()->GetMesh()->GetSocketLocation(TEXT("head"));
		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(PawnRotation, AimRotation);
		Pitch = UKismetMathLibrary::InverseTransformRotation(Character->GetActorTransform(), LookRotation).Pitch;
	}
}

void UAIAnimInstance::UpdateBowValue()
{
	FRotator ConRot = Character->GetConRot();
	FRotator ActorRot = Character->GetActorRotation();
	
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(ConRot, ActorRot);

	BowPitch = FMath::Clamp(DeltaRot.Pitch, -50.f, 50.f);
	if(Character->GetVelocity().Size() <= 0.f) BowYaw = FMath::Clamp(DeltaRot.Yaw, -50.f, 50.f);
	else BowYaw = 0.f;
}

