// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PlayerCameraManagerBase.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API APlayerCameraManagerBase : public APlayerCameraManager
{
	GENERATED_BODY()
	
protected:
	APlayerCameraManagerBase(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION()
	void RunLowCameraShake();
	UFUNCTION()
	void RunHeavyCameraShake();

	UFUNCTION()
	void LockTopView();
	UFUNCTION()
	void InGameView();

	UFUNCTION()
	void BindPossessPawn(class APlayerCharacter* InPawn);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraShake")
	TSubclassOf<class UCameraShakeBase> LowShake_C;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraShake")
	TSubclassOf<class UCameraShakeBase> HeavyShake_C;

private:
	class APlayerCharacter* PossessPawn;
};
