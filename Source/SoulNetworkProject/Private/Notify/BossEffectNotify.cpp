// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/BossEffectNotify.h"
#include "Classes/BossCharacter.h"

FString UBossEffectNotify::GetNotifyName_Implementation() const
{
	return L"Boss Effect";
}

void UBossEffectNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ABossCharacter* Boss = Cast<ABossCharacter>(MeshComp->GetOwner()))
	{
		if (Boss->GetBossManager())
		{
		}
	}
}
