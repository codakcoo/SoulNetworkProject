// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/DamagePossibleNotifyState.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"

FString UDamagePossibleNotifyState::GetNotifyName_Implementation() const
{
	return L"Damage Possible";
}

void UDamagePossibleNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetStateComponent())
		{
			Chr->GetStateComponent()->SetDamagePossible(false);
		}
	}
}

void UDamagePossibleNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetStateComponent())
		{
			Chr->GetStateComponent()->SetDamagePossible(true);
		}
	}
}
