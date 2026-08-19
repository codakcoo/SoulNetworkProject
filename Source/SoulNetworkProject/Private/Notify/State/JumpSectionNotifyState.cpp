// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/JumpSectionNotifyState.h"
#include "Classes/CharacterBase.h"

#include "Animation/AnimInstance.h"
#include "Gameframework/CharacterMovementComponent.h"

FString UJumpSectionNotifyState::GetNotifyName_Implementation() const
{
	return L"Jump Section";
}

void UJumpSectionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation,TotalDuration);

	if (JumpSection == NAME_None) return;
	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
		{
			if (AnimInstance->GetCurrentActiveMontage())
			{
				if (Condition == EJumpCondition::Timer)
				{
					GetWorld()->GetTimerManager().ClearTimer(JumpTimer);
					GetWorld()->GetTimerManager().SetTimer(JumpTimer, FTimerDelegate::CreateWeakLambda(this, [&Chr, &AnimInstance, this]
					{
							Chr->Server_InteractionMontage(AnimInstance->GetCurrentActiveMontage(), this->JumpSection);
					}), JumpDelay, false);
				}

			}
		}
	}
}

void UJumpSectionNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if(JumpSection == NAME_None) return;
	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if(UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
		{
			if (AnimInstance->GetCurrentActiveMontage())
			{
				if (Condition == EJumpCondition::Falling)
				{
					if(!Chr->GetCharacterMovement()->IsFalling())
					Chr->Server_InteractionMontage(AnimInstance->GetCurrentActiveMontage(), JumpSection);
				}

			}
		}
	}
}

void UJumpSectionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (JumpSection == NAME_None) return;
	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
		{
			if (AnimInstance->GetCurrentActiveMontage())
			{
				if (Condition == EJumpCondition::Trigger)
				{
					Chr->Server_InteractionMontage(AnimInstance->GetCurrentActiveMontage(), JumpSection);
				}
			}
		}
	}
}
