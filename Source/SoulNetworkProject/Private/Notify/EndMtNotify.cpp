// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/EndMtNotify.h"
#include "Classes/CharacterBase.h"

FString UEndMtNotify::GetNotifyName_Implementation() const
{
	return L"End Mt";
}

void UEndMtNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->EndMt();
	}
}
