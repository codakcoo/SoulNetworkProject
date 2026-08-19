// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/HyperArmorNotifyState.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"

FString UHyperArmorNotifyState::GetNotifyName_Implementation() const
{
	return L"Hyper Armor";
}

void UHyperArmorNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->GetStateComponent()->SetUseHyperArmor(true);
	}
}

void UHyperArmorNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->GetStateComponent()->ResetPoiseStat();
	}
}
