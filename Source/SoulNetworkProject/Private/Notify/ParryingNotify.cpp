// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/ParryingNotify.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"

FString UParryingNotify::GetNotifyName_Implementation() const
{
	return L"Parrying_Notify";
}

void UParryingNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Owner = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Owner->GetCombatComponent())
		{
			Owner->GetCombatComponent()->SetParryingParam(ParryingTime);
		}
	}
}
