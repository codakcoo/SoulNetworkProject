// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/DamagePossibleNotify.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"	

FString UDamagePossibleNotify::GetNotifyName_Implementation() const
{
	return L"Damage Possible";
}

void UDamagePossibleNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->GetStateComponent()->OnDamagePossible(PossibleDelay);
	}
}
