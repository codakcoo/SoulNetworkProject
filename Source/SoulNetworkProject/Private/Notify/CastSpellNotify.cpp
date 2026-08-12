// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/CastSpellNotify.h"
#include "Classes/CharacterBase.h"

FString UCastSpellNotify::GetNotifyName_Implementation() const
{
	return L"Cast Spell";
}

void UCastSpellNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetLocalRole() == ROLE_Authority)
		{
			if (CastType == ESpellCastType::Begin) Chr->Server_SpawnCastSpell();
			else if(CastType == ESpellCastType::Loop && Chr->GetSpellActor()) Chr->Server_LoopCastSpell();
			else if (CastType == ESpellCastType::End && Chr->GetSpellActor()) Chr->Server_EndCastSpell();
		}
	}
}
