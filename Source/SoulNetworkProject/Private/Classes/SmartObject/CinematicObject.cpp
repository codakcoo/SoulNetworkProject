// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/CinematicObject.h"

#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ACinematicObject::ACinematicObject()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACinematicObject::BeginPlay()
{
	Super::BeginPlay();
}
void ACinematicObject::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

USkeletalMeshComponent* ACinematicObject::GetMesh()
{
	TArray<UActorComponent*> OutComponents;
	OutComponents = GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("MESH"));

	return OutComponents.Num() != 0 ? Cast<USkeletalMeshComponent>(OutComponents[0]) : nullptr;
}

USkeletalMeshComponent* ACinematicObject::GetWeaponMesh()
{
	TArray<UActorComponent*> OutComponents;
	OutComponents = GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("WEAPON"));

	return OutComponents.Num() != 0 ? Cast<USkeletalMeshComponent>(OutComponents[0]) : nullptr;
}

void ACinematicObject::OnNIagara(class USkeletalMeshComponent* Mesh)
{
	if (Mesh)
	{
		if (UFXSystemComponent* FXComponent = SpawnNiagara(Mesh))
		{
			if (UNiagaraComponent* NSComponent = Cast<UNiagaraComponent>(FXComponent))
			{
				NSComponent->SetVariableObject("SkeletaltoAttach", Mesh);
			}
		}
	}
}
void ACinematicObject::OffNiagara(class USkeletalMeshComponent* Mesh)
{
	if (Mesh)
	{
		TArray<USceneComponent*> Childrens;
		Mesh->GetChildrenComponents(false, Childrens);

		if (Childrens.Num())
		{
			for (USceneComponent* Component : Childrens)
			{
				if (Component && Component->ComponentHasTag(TEXT("ENCHANT")))
				{
					if (UFXSystemComponent* FXComponent = Cast<UFXSystemComponent>(Component))
					{
						FXComponent->ComponentTags.Remove(TEXT("ENCHANT"));
						FXComponent->Deactivate();
					}
				}
			}
		}
	}
}

UFXSystemComponent* ACinematicObject::SpawnNiagara(class USkeletalMeshComponent* Mesh)
{
	if (Mesh && Asset)
	{
		UFXSystemComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAttached(
			Asset,
			Mesh,
			TEXT("Enchant"),
			Mesh->GetSocketLocation(TEXT("Enchant")),
			UKismetMathLibrary::MakeRotFromZ(Mesh->GetSocketLocation(TEXT("Enchant"))), EAttachLocation::KeepWorldPosition, false);
	
		Component->ComponentTags.AddUnique(TEXT("ENCHANT"));
	
		return Component;
	}
	
	return nullptr;
}