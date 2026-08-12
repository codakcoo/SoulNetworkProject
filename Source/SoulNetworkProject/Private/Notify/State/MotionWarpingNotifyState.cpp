// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/MotionWarpingNotifyState.h"
#include "Classes/CharacterBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/CombatComponent.h"

#include "Gameframework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

FString UMotionWarpingNotifyState::GetNotifyName_Implementation() const
{
	return L"Motion Warping";
}

void UMotionWarpingNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if(ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->GetCharacterMovement()->SetMovementMode(BeginMovementMode);

		if (HomingType == EHomingType::None)
		{
			TargetLocation = Chr->GetActorLocation() + (Chr->GetActorForwardVector() * WrapVector.X) + (Chr->GetActorRightVector() * WrapVector.Y) + (Chr->GetActorUpVector() * WrapVector.Z);
		}
		else
		{
			if (Chr->GetFocusActor())
			{
				TargetLocation = Chr->GetFocusActor()->GetActorLocation();
			}
			//else
			//{
			//	if (APlayerCharacter* Player = Cast<APlayerCharacter>(Chr))
			//	{
			//		
			//		//else
			//		//{
			//		//	TargetLocation = End;
			//		//}
			//		if (Player->GetRemoteRole() == ROLE_Authority)
			//		{
			//			Player->GetCombatComponent()->GetForwardVector(TargetLocation);
			//		}
			//		else if(Player->GetLocalRole() == ROLE_AutonomousProxy)
			//		{
			//			Player->GetCombatComponent()->Client_GetForwardVector(TargetLocation);
			//		}
			//	}
			//
			//	//TargetLocation = FVector::ZeroVector;
			//
			//
			//}
		}
	}
}

void UMotionWarpingNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (TargetLocation == FVector::ZeroVector) return;
	else if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		FVector CurrentLocation = Chr->GetActorLocation();

		CurrentLocation = UKismetMathLibrary::VInterpTo(CurrentLocation, TargetLocation, FrameDeltaTime, InterpSpeed);
		Chr->SetActorLocation(CurrentLocation);
	}
}

void UMotionWarpingNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->GetCharacterMovement()->SetMovementMode(EndMovementMode);
	}
}
