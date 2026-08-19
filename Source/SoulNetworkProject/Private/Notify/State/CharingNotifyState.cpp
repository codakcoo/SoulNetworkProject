// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/CharingNotifyState.h"
#include "Classes/CharacterBase.h"


FString UCharingNotifyState::GetNotifyName_Implementation() const
{
	return L"Charing_State";
}

void UCharingNotifyState::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->SetCharging(true);
	}
}

void UCharingNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->SetCharging(false);
	}
}