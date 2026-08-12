// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AllMeshNiagaraNotify.h"
#include "Classes/CharacterBase.h"
#include "Classes/EffectComponent.h"

FString UAllMeshNiagaraNotify::GetNotifyName_Implementation() const
{
	return L"All_Mesh_Niagara";
}

void UAllMeshNiagaraNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetEffectComponent())
		{
			Chr->GetEffectComponent()->RunBodyMeshNiagara(TEXT("Dead"));
		}
	}
}
