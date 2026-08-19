// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/StrikeSpellCollisionNotifyState.h"
#include "Classes/PlayerCharacter.h"

FString UStrikeSpellCollisionNotifyState::GetNotifyName_Implementation() const
{
	return L"StrikeSpell Collision";
}

void UStrikeSpellCollisionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		if (Player->GetRemoteRole() == ROLE_AutonomousProxy || Player->GetLocalRole() == ROLE_Authority)
		{
			Player->Server_LoopCastSpell();

		}
	}
}

void UStrikeSpellCollisionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		if (Player->GetRemoteRole() == ROLE_AutonomousProxy || Player->GetLocalRole() == ROLE_Authority)
		{

			//Player->Server_FalseLoopCastSpell();
			Player->Server_EndCastSpell();
		}
	}
}
