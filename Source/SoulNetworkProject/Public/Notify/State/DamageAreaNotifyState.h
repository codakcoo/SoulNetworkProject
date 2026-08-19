// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Classes/CombatComponent.h"
#include "DamageAreaNotifyState.generated.h"

/**
 * 
 */


UCLASS()
class SOULNETWORKPROJECT_API UDamageAreaNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;

public:
	UPROPERTY(EditAnywhere, Category = "Socket")
	FName SocketName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Value")
	float AreaRadius = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Value")
	EAttackType AreaDamageType = EAttackType::None;
};
