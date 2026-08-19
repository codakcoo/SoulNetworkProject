// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/LoudEventNotify.h"
#include "Classes/CharacterBase.h"

#include "Kismet/GameplayStatics.h"

FString ULoudEventNotify::GetNotifyName_Implementation() const
{
	return L"Loud Event";
}

void ULoudEventNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->UpdatedLoudNoiseEvent(SoundAsset);
	}
}
