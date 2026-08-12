// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/CameraShakeAreaNotify.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"


FString UCameraShakeAreaNotify::GetNotifyName_Implementation() const
{
	return L"Camera Shake Area";
}


void UCameraShakeAreaNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if(!Chr->GetCombatComponent()) return;

		FVector SpawnLocation = MeshComp->GetSocketLocation(SpawnBoneName);
		if(SpawnLocation == FVector::ZeroVector) SpawnLocation = MeshComp->GetSocketLocation(TEXT("spine_02"));

		if (Chr->GetRemoteRole() == ROLE_AutonomousProxy || Chr->GetLocalRole() == ROLE_Authority)
		{
			Chr->GetCombatComponent()->UpdatedCameraShakeArea(SpawnLocation, AreaRadius, EAttackType::Heavy);
		}
	}
}
