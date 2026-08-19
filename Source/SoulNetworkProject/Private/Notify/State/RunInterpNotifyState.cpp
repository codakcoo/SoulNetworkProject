// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/RunInterpNotifyState.h"
#include "Classes/CharacterBase.h"

FString URunInterpNotifyState::GetNotifyName_Implementation() const
{
	return L"Run_Interp";
}

void URunInterpNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacterBase* Owner = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Owner->Server_SetUseOrientYaw(true);
	}
}

void URunInterpNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Owner = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Owner->Server_SetUseOrientYaw(false);
	}
}
