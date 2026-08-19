// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Magic/BuffSpellActor.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/EffectComponent.h"


ABuffSpellActor::ABuffSpellActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

//���� ȣ��//
void ABuffSpellActor::BeginData(ACharacterBase* InOwner, USkeletalMeshComponent* CastMesh)
{
	if (!InOwner) return;
	//const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	SpawnOwner = InOwner;
	IgnoreActors.Add(SpawnOwner);
	SpellStateType = ESpellStateType::Begin;


	//if (InOwner->GetSpellData().SpellData.SpellType == ESpellType::AttackSorcery || InOwner->GetSpellData().SpellData.SpellType == ESpellType::AssistSorcery)
	//{
	//	if (InOwner->GetMainWeapon().IsSet())
	//	{
	//		Multicast_BeginData();
	//		this->AttachToComponent(SpawnOwner->GetMainWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
	//	}
	//}
	//else if (InOwner->GetSpellData().SpellData.SpellType == ESpellType::AttackIncantation || InOwner->GetSpellData().SpellData.SpellType == ESpellType::AssistIncantation)
	//{
	//	if (InOwner->GetSecondaryWeapon().IsSet())
	//	{
	//		this->AttachToComponent(SpawnOwner->GetSecondaryWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
	//	}
	//}
			Multicast_BeginData();
}
//////////

//�߰� ȣ��//
void ABuffSpellActor::LoopData()
{
	if (!SpawnOwner) return;

	//bUseLoop = true;
	SpellStateType = ESpellStateType::Loop;
	SpawnOwner->GetStateComponent()->AddMana(-10.f);

	SpawnOwner->GetStateComponent()->RunBuffData(BuffName);

	Multicast_LoopData();
}
////////////

//�� ȣ��///
void ABuffSpellActor::EndData()
{
	SpawnOwner->Server_ClearSpellActor();

	GetWorld()->GetTimerManager().ClearTimer(LifeTimer);
	Destroy();
}
///////////
