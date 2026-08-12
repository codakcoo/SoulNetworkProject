// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/ExecutionDamageNotify.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"

FString UExecutionDamageNotify::GetNotifyName_Implementation() const
{
	return L"Execution Damage";
}


void UExecutionDamageNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetCombatComponent())
		{
			Chr->GetCombatComponent()->ExecutionDamage();
		}
	}
}
