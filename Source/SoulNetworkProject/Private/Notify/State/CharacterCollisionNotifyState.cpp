// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/CharacterCollisionNotifyState.h"
#include "Classes/CharacterBase.h"

#include "Components/CapsuleComponent.h"

FString UCharacterCollisionNotifyState::GetNotifyName_Implementation() const
{
	return L"Chr Collision State";
}

void UCharacterCollisionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->GetCapsuleComponent()->SetCollisionProfileName(BeginProfileName);
	}
}

void UCharacterCollisionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->GetCapsuleComponent()->SetCollisionProfileName(EndProfileName);
	}
}
