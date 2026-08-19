// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/DamageAreaNotifyState.h"
#include "Classes/CharacterBase.h"

#include "Kismet/KismetSystemLibrary.h"

FString UDamageAreaNotifyState::GetNotifyName_Implementation() const
{
	return L"Damage Area";
}


void UDamageAreaNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetCombatComponent())
		{
			FVector SocketLocation = Chr->GetMesh()->GetSocketLocation(SocketName);
			Chr->GetCombatComponent()->SphereDamageArea(SocketLocation, AreaRadius, AreaDamageType);
		}
	}

	//if (MeshComp->GetSocketByName(SocketName))
	//{
	//	FVector AreaLocation = MeshComp->GetSocketLocation(SocketName);
	//	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), AreaLocation, AreaRadius, 12, FLinearColor(1.f, 1.f, 1.f), 5.0f);
	//}

}
