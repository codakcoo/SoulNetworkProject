// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/OutofControlNotifyState.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"

FString UOutofControlNotifyState::GetNotifyName_Implementation() const
{
	return L"Out_of_Control";
}

void UOutofControlNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetCombatComponent())
		{
			Chr->GetCombatComponent()->SetOutofControl(true);
		}
	}
}

void UOutofControlNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetCombatComponent())
		{
			//Chr->GetCombatComponent()->UpdatedExecutionTrace(F_SocketName, B_SocketName);
		}
	}
}

void UOutofControlNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetCombatComponent())
		{
			Chr->GetCombatComponent()->SetOutofControl(false);
		}
	}
}
