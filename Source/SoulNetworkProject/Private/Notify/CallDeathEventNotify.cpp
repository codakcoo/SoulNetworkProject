// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/CallDeathEventNotify.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"
#include "Classes/CharacterStateComponent.h"

FString UCallDeathEventNotify::GetNotifyName_Implementation() const
{
	return L"Call Death Event";
}

void UCallDeathEventNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetCombatComponent() && Chr->GetStateComponent())
		{
			if(Chr->GetStateComponent()->GetCurrentHealth() <= 0.0f)
				Chr->GetCombatComponent()->CallDeathEvent(Delay);
		}
	}
}
