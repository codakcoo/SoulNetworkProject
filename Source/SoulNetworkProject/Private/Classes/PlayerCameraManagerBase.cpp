// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/PlayerCameraManagerBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"

#include "Gameframework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

APlayerCameraManagerBase::APlayerCameraManagerBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void APlayerCameraManagerBase::RunLowCameraShake()
{
	//GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake();
	StartCameraShake(LowShake_C, 1.0f);
}

void APlayerCameraManagerBase::RunHeavyCameraShake()
{
	StartCameraShake(HeavyShake_C, 1.0f);
}

void APlayerCameraManagerBase::LockTopView()
{
	if (PossessPawn && PossessPawn->GetCamera() && PossessPawn->GetTopViewCamera())
	{
		PossessPawn->GetCamera()->SetActive(false);
		PossessPawn->GetTopViewCamera()->SetActive(true);
	}
}

void APlayerCameraManagerBase::InGameView()
{
	if (PossessPawn && PossessPawn->GetCamera() && PossessPawn->GetTopViewCamera())
	{
		PossessPawn->GetCamera()->SetActive(true);
		PossessPawn->GetTopViewCamera()->SetActive(false);
	}
}

void APlayerCameraManagerBase::BindPossessPawn(APlayerCharacter* InPawn)
{
	if (InPawn)
	{
		PossessPawn = InPawn;
	}
}
