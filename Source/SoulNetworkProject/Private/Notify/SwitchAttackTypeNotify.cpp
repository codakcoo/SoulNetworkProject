// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/SwitchAttackTypeNotify.h"
#include "Classes/CharacterBase.h"

FString USwitchAttackTypeNotify::GetNotifyName_Implementation() const
{
	return L"Switch_Attack";
}

void USwitchAttackTypeNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	//if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	//{
	//	if (Chr->GetCombatComponent())
	//	{
	//		Chr->GetCombatComponent()->SetAttackType(AttackType);
	//	}
	//}
}
