// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/PlayEffectNotify.h"
#include "Classes/CharacterBase.h"
#include "Classes/EffectComponent.h"

FString UPlayEffectNotify::GetNotifyName_Implementation() const
{
	return L"Play Effect";
}

void UPlayEffectNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetEffectComponent())
		{
			FVector PlayLocation = Chr->GetMesh()->GetSocketLocation(SocketName);
			Chr->GetEffectComponent()->Server_ReactionEffect(SoundAsset, PaticleAsset, PlayLocation, FRotator::ZeroRotator, EffectScale);
		}
	}
}
