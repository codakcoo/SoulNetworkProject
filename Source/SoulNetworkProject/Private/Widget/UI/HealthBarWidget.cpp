// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UI/HealthBarWidget.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/CharacterStateComponent.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

bool UHealthBarWidget::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	//if(HealthBar) HealthBar->PercentDelegate.BindUFunction(this, "OnUpdatedHealthPercent");

	return true;
}

void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Owner = Cast<ACharacterBase>(GetOwningPlayer()->GetPawn());
	PlayerNameText->SetVisibility(ESlateVisibility::Hidden);
}

void UHealthBarWidget::OnUpdatedHealthPercent(float MaxHealth, float CurrentHealth)
{
	if(!Owner) return;
	
	//float Percent = 0.f;

	//Percent = ;
	HealthBar->SetPercent(CurrentHealth / MaxHealth);

	//UE_LOG(LogTemp, Warning, TEXT("%f"), Percent);
}

void UHealthBarWidget::SetupNameText(FName PlayerName)
{
	if(!PlayerNameText) return;

	PlayerNameText->SetVisibility(ESlateVisibility::Visible);
	PlayerNameText->SetText(FText::FromName(PlayerName));
}
