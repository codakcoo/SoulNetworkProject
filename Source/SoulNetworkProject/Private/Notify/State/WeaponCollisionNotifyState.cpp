// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/WeaponCollisionNotifyState.h"

#include "Classes/CharacterBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/EffectComponent.h"

void UWeaponCollisionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	//if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	//{
	//	if (Chr->GetCombatComponent())
	//	{
	//		Chr->GetCombatComponent()->SetAttackType(PermuteType);
	//	}
	//}
}


void UWeaponCollisionNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		//Chr->OnWeaponTrace(HandType);
		if (Chr->GetCombatComponent())
		{
			//if(Chr->GetRemoteRole() == ROLE_AutonomousProxy) Chr->GetCombatComponent()->Server_WeaponTrace(HandType, StartSocket, EndSocket);
			//else if(Chr->GetLocalRole() == ROLE_Authority) Chr->GetCombatComponent()->WeaponTrace(HandType, StartSocket, EndSocket);
			Chr->GetCombatComponent()->WeaponTrace(HandType, PermuteType, StartSocket, EndSocket);
		}

		//Chr->Server_OnWeaponTrace(HandType);
	}
}

void UWeaponCollisionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	//if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	//{
	//	if (Chr->GetCombatComponent())
	//	{
	//		Chr->GetCombatComponent()->SetAttackType(EAttackType::None);
	//	}
	//}
}
