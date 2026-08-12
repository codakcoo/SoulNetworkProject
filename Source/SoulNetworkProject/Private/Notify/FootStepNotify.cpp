// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/FootStepNotify.h"
#include "Classes/CharacterBase.h"
#include "Classes/EffectComponent.h"

#include "Gameframework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

FString UFootStepNotify::GetNotifyName_Implementation() const
{
	return L"Foot Step";
}


void UFootStepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if(bUseEvent)
		{
			if(!Chr->AnimValues.bCrouch) Chr->UpdatedStepEvent(SocketName, bUseEvent, 1.0f);
		}
		else Chr->UpdatedStepEvent(SocketName, false, 0.5f);
	}
}
