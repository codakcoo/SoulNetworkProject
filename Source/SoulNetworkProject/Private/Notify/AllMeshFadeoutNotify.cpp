// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AllMeshFadeoutNotify.h"
#include "Classes/CharacterBase.h"

FString UAllMeshFadeoutNotify::GetNotifyName_Implementation() const
{
	return L"All Mesh Fadeout";
}

void UAllMeshFadeoutNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);


	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if(Chr->GetUseFadeMesh()) Chr->FadeInOut(RunType == EFadeoutType::In ? true : false);
	}
}
