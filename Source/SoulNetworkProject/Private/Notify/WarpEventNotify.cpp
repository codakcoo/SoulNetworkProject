// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/WarpEventNotify.h"
#include "Classes/PlayerCharacter.h"
#include "SoulNetworkProjectPlayerState.h"

#include "Interface/CharacterInterface.h"

FString UWarpEventNotify::GetNotifyName_Implementation() const
{
	return L"Warp Event";
}

void UWarpEventNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (APlayerCharacter* Chr = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		if(!Chr->GetPlayerState()) return;
		else if (Chr->GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{

			Cast<ICharacterInterface>(Chr->GetPlayerState())->UpdatedWarpPoint();
		}
	}
}
