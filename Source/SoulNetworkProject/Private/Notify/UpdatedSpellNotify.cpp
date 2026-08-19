// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/UpdatedSpellNotify.h"
#include "Classes/CharacterBase.h"
#include "Classes/Magic/MasterMagicActor.h"

#include "SoulNetworkProject/ItemData.h"

#include "Interface/CombatInterface.h"

FString UUpdatedSpellNotify::GetNotifyName_Implementation() const
{
	return L"Updated Spell";
}

void UUpdatedSpellNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if(!Chr->AnimValues.bSpellAttack && !Chr->GetSpellActor()) return;
		if (Chr->GetLocalRole() == ROLE_Authority && Chr->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			Cast<ICombatInterface>(Chr)->UpdatedSpell(Chr->GetSpellData().SpellData.ManaCost);
		}
	}
}
