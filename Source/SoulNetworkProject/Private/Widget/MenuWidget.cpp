// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MenuWidget.h"

void UMenuWidget::Setup()
{
	AddToViewport();

	APlayerController* Con = GetWorld()->GetFirstPlayerController();
	if (!Con) return;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
	
	Con->SetInputMode(InputModeData);
	Con->bShowMouseCursor = true;
}

void UMenuWidget::Teardown()
{
	RemoveFromViewport();

	APlayerController* Con = GetWorld()->GetFirstPlayerController();
	if (!Con) return;

	FInputModeGameOnly InputModeData;
	//InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);

	Con->SetInputMode(InputModeData);
	Con->bShowMouseCursor = false;
}
