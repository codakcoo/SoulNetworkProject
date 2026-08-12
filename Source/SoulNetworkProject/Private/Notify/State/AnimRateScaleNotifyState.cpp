// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/AnimRateScaleNotifyState.h"
#include "Classes/CharacterBase.h"

FString UAnimRateScaleNotifyState::GetNotifyName_Implementation() const
{
	return L"Anim_Rate_Scale";
}

void UAnimRateScaleNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetMesh())
		{
			Chr->GetMesh()->GlobalAnimRateScale = ScaleRate;
		}
	}
}

void UAnimRateScaleNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetMesh())
		{
			Chr->GetMesh()->GlobalAnimRateScale = 1.f;
		}
	}
}
