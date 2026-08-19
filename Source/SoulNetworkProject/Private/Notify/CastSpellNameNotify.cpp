// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/CastSpellNameNotify.h"
#include "Classes/CharacterBase.h"

#include "Interface/CombatInterface.h"


FString UCastSpellNameNotify::GetNotifyName_Implementation() const
{
	return L"Cast Spell Name";
}

void UCastSpellNameNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetLocalRole() == ROLE_Authority)
		{

			USkeletalMeshComponent* CaseMeshComp = nullptr;
			switch (CastMesh)
			{
				case ECastMesh::BodyMesh:
					CaseMeshComp = Chr->GetMesh();
					break;
				case ECastMesh::MainWeaponMesh:
					CaseMeshComp = Chr->GetMainWeaponMesh();
					break;
				case  ECastMesh::SecondaryWeaponMesh:
					CaseMeshComp = Chr->GetSecondaryWeaponMesh();
					break;	
			}

			if(CaseMeshComp) Chr->Server_SpawnCastSpellName(CaseMeshComp, SpellName, SpawnSocket);
		}
	}
}
