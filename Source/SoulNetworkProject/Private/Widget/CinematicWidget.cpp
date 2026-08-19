// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CinematicWidget.h"
#include "Classes/PlayerCharacter.h"

#include "Interface/GameStateInterface.h"

#include "Components/Image.h"

UCinematicWidget::UCinematicWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UCinematicWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	VisibilityDelegate.BindUFunction(this,"IsActiveInGameMenu");

	return true;
}

void UCinematicWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		Owner = Player;

		bPressSpace = false;
		CurrentSkip = 0.0f;
	}

}

void UCinematicWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(bPlay) UpdatedPercent(InDeltaTime);
}

void UCinematicWidget::StartCinematic()
{
	PlayAnimation(BlinkAnim);
	SetPercentInMaterial(0.0f);
}

void UCinematicWidget::SetPercent(bool bActive)
{
	bPressSpace = bActive;
	bPlay = true;
	if(bPressSpace)
	{
		StopAnimation(BlinkAnim);
		PlayAnimation(VisibleAnim);
	}
}

ESlateVisibility UCinematicWidget::IsActiveInGameMenu()
{
	if (!Owner || !Owner->GetHUD()) return ESlateVisibility::Hidden;

	return Owner->GetHUD()->GetActiveMenu() != EActiveMenu::Cinematic ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible;
}

void UCinematicWidget::UpdatedPercent(float InDeltaTime)
{
	UMaterialInstanceDynamic* DynamicMaterial = ProgressCircle->GetDynamicMaterial();
	if (DynamicMaterial && bPlay)
	{
		if(bPressSpace) CurrentSkip += InDeltaTime;
		else CurrentSkip -= InDeltaTime;

		SetPercentInMaterial(CurrentSkip / MaxSkip);

		if(CurrentSkip >= MaxSkip && bPressSpace) FillPercent();
		else if(CurrentSkip <= 0.0f && bPressSpace) ClearPercent();
	}
}

void UCinematicWidget::FillPercent()
{
	if (!GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass())) return;
	
	bPlay = false;
	Cast<IGameStateInterface>(GetWorld()->GetGameState())->SkipCinematic();
	PlayAnimation(HiddenAnim);
}

void UCinematicWidget::ClearPercent()
{
	bPlay = false;
	PlayAnimation(HiddenAnim);
}

void UCinematicWidget::SetPercentInMaterial(float Value)
{
	UMaterialInstanceDynamic* DynamicMaterial = ProgressCircle->GetDynamicMaterial();
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue("Percent", Value);
	}
}
