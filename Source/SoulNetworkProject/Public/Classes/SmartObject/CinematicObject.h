// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "CinematicObject.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ACinematicObject : public ASmartObject
{
	GENERATED_BODY()
	
protected:
	ACinematicObject();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

public:
	// That should get of set name to 'MESH' for TagName.
	UFUNCTION()
	class USkeletalMeshComponent* GetMesh();
	// That should get of set name to 'WEAPON' for TagName.
	UFUNCTION()
	class USkeletalMeshComponent* GetWeaponMesh();

	UFUNCTION()
	void OnNIagara(class USkeletalMeshComponent* Mesh);
	UFUNCTION()
	void OffNiagara(class USkeletalMeshComponent* Mesh);

private:
	UFUNCTION()
	class UFXSystemComponent* SpawnNiagara(class USkeletalMeshComponent* Mesh);

public:
	UPROPERTY(EditAnywhere, Category = "Asset")
	class UNiagaraSystem* Asset;
};
