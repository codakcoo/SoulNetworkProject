// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Magic/MasterMagicActor.h"
#include "BuffSpellActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ABuffSpellActor : public AMasterMagicActor
{
	GENERATED_BODY()

protected:
	ABuffSpellActor();

public:
	UFUNCTION()
	virtual void BeginData(class ACharacterBase* InOwner, class USkeletalMeshComponent* CastMesh) override;

	UFUNCTION()
	virtual void LoopData() override;

	UFUNCTION()
	virtual void EndData() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	FName BuffName;
};
