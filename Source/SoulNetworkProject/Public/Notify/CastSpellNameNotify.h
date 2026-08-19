// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CastSpellNameNotify.generated.h"

UENUM(BlueprintType)
enum class ECastMesh : uint8
{
	BodyMesh				UMETA(DisplayName = "BodyMesh"),
	MainWeaponMesh			UMETA(DisplayName = "MainWeaponMesh"),
	SecondaryWeaponMesh		UMETA(DisplayName = "SecondaryWeaopnMesh")
};
/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UCastSpellNameNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "Value")
	ECastMesh CastMesh;
	UPROPERTY(EditAnywhere, Category = "Value")
	FName SpellName;
	UPROPERTY(EditAnywhere, Category = "Value")
	FName SpawnSocket = TEXT("Spell_Socket");
};
