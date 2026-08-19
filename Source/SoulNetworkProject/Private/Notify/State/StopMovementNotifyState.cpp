// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/StopMovementNotifyState.h"

#include "Classes/PlayerCharacter.h"
#include "Gameframework/CharacterMovementComponent.h"

void UStopMovementNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (APlayerCharacter* Chr = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		//Owner = Chr;
		Chr->SetHasCrowdControl(true);
	}
}

void UStopMovementNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (APlayerCharacter* Chr = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		Chr->SetHasCrowdControl(false);
		//Owner = nullptr;
	}
}
