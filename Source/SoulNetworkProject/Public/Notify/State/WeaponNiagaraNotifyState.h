// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Particles/ParticleSystemComponent.h"
#include "WeaponNiagaraNotifyState.generated.h"

/**
 * 
 */
 

UCLASS()
class SOULNETWORKPROJECT_API UWeaponNiagaraNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


private:
	void TrailEffect(class ACharacterBase* InChr);
	void RemoveTrailEffect(class ACharacterBase* InChr);

	UFXSystemComponent* SpawnEffect(USkeletalMeshComponent* ParentMesh);
	UFXSystemComponent* GetSpawnEffect(USkeletalMeshComponent* ParentMesh);

	FORCEINLINE FName GetSpawnedComponentTag() const { return GetFName(); }

public:
	UPROPERTY(EditAnywhere, Category = "Asset")
	class UNiagaraSystem* WeaponNiagaraAsset;
};
