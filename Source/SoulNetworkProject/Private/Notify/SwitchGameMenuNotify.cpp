// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/SwitchGameMenuNotify.h"
#include "Classes/PlayerControllerBase.h"


FString USwitchGameMenuNotify::GetNotifyName_Implementation() const
{
	return L"Switch Menu";
}

void USwitchGameMenuNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (APlayerControllerBase* PC = Cast<APlayerControllerBase>(MeshComp->GetOwner()->GetInstigatorController()))
	{
		PC->CallCheckPointMenu();
	}
}
