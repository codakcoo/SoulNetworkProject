// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/State/WeaponNiagaraNotifyState.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

FString UWeaponNiagaraNotifyState::GetNotifyName_Implementation() const
{
	return L"Weapon_NIagara";
}

void UWeaponNiagaraNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		TrailEffect(Chr);
	}
}

void UWeaponNiagaraNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		//if (Chr->GetMainWeapon().IsSet())
		//{
		//}
			RemoveTrailEffect(Chr);

	}
}

void UWeaponNiagaraNotifyState::TrailEffect(ACharacterBase* InChr)
{
	RemoveTrailEffect(InChr);

	//if (InChr->GetMainWeapon().IsSet())
	{
		if (UFXSystemComponent* Component = SpawnEffect(InChr->GetMainWeaponMesh()))
		{
			// tag the component with the AnimNotify that is triggering the animation so that we can properly clean it up
			Component->ComponentTags.AddUnique(GetSpawnedComponentTag());

		}
	}
}

void UWeaponNiagaraNotifyState::RemoveTrailEffect(ACharacterBase* InChr)
{
	if (UFXSystemComponent* FXComponent = GetSpawnEffect(InChr->GetMainWeaponMesh()))
	{
		// untag the component//
		FXComponent->ComponentTags.Remove(GetSpawnedComponentTag());
		FXComponent->Deactivate();

		// Either destroy the component or deactivate it to have it's active FXSystems finish.
		// The component will auto destroy once all FXSystem are gone.
		//if (bDestroyAtEnd)
		//{
		//FXComponent->DestroyComponent();
		//}
		//else
		//{
		//}
	}
}

UFXSystemComponent* UWeaponNiagaraNotifyState::SpawnEffect(USkeletalMeshComponent* ParentMesh)
{
	if (ParentMesh)
	{
		return UNiagaraFunctionLibrary::SpawnSystemAttached(WeaponNiagaraAsset, ParentMesh,
			FName("Trail_Socket"), FVector(), UKismetMathLibrary::MakeRotFromZ(ParentMesh->GetSocketLocation("Trail_Socket")), FVector(1.f),
			EAttachLocation::SnapToTarget, false, ENCPoolMethod::None);
	}
	return nullptr;
}

UFXSystemComponent* UWeaponNiagaraNotifyState::GetSpawnEffect(USkeletalMeshComponent* ParentMesh)
{
	if (ParentMesh)
	{
		TArray<USceneComponent*> Children;
		ParentMesh->GetChildrenComponents(false, Children);

		if (Children.Num())
		{
			for (USceneComponent* Component : Children)
			{
				if (Component && Component->ComponentHasTag(GetSpawnedComponentTag()))
				{
					if (UFXSystemComponent* FXComponent = CastChecked<UFXSystemComponent>(Component))
					{
						return FXComponent;
					}
				}
			}
		}
	}

	return nullptr;
}
