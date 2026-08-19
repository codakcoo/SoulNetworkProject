// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/InputCheckNotifyState.h"

#include "Classes/CharacterBase.h"


FString UInputCheckNotifyState::GetNotifyName_Implementation() const
{
	return L"Input Check";
}

void UInputCheckNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		//Chr->EndInputChecking();
		Chr->SetCanInputCheck(true);
	}
}

void UInputCheckNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->EndInputChecking();
	}
}

void UInputCheckNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		//Chr->EndInputChecking();
		Chr->SetCanInputCheck(false);
	}
}
