// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/BossEffectNotifyState.h"
#include "Classes/BossCharacter.h"
#include "Classes/BossManager.h"

FString UBossEffectNotifyState::GetNotifyName_Implementation() const
{
	return L"Boss Effect";
}

void UBossEffectNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ABossCharacter* Chr = Cast<ABossCharacter>(MeshComp->GetOwner()))
	{
		if (Chr->GetBossManager())
		{
			RunType == EEffectObjectRunType::Single ? Chr->GetBossManager()->SingleEffect(true) : Chr->GetBossManager()->MultiEffect(true);
		}
	}
}

void UBossEffectNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ABossCharacter* Chr = Cast<ABossCharacter>(MeshComp->GetOwner()))
	{
		if (Chr->GetBossManager())
		{
			RunType == EEffectObjectRunType::Single ? Chr->GetBossManager()->SingleEffect(false) : Chr->GetBossManager()->MultiEffect(false);
		}
	}
}
