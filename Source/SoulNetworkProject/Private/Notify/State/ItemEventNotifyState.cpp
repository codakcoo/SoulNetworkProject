// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/ItemEventNotifyState.h"

FString UItemEventNotifyState::GetNotifyName_Implementation() const
{
	return L"Item_Event";
}

void UItemEventNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);


}

void UItemEventNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
}
