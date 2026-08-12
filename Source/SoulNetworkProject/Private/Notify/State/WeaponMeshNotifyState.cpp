// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/WeaponMeshNotifyState.h"
#include "Classes/CharacterBase.h"

FString UWeaponMeshNotifyState::GetNotifyName_Implementation() const
{
	return L"Weapon Mesh";
}

void UWeaponMeshNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if(bHiddenLeft)
		{
			Chr->GetMainWeaponMesh()->SetHiddenInGame(true);
			Chr->GetMainShieldMesh()->SetHiddenInGame(true);
			Chr->GetBowWeaponMesh()->SetHiddenInGame(true);
		}
		if (bHiddenRight)
		{
			Chr->GetSecondaryWeaponMesh()->SetHiddenInGame(true);
			Chr->GetSecondaryShieldMesh()->SetHiddenInGame(true);
		}
	}
}

void UWeaponMeshNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Chr->UpdatedWeaponMesh();
	}
}
