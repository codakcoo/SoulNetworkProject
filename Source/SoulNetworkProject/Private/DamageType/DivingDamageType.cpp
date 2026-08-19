// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageType/DivingDamageType.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/SmartObject/WaterArea.h"

void UDivingDamageType::ApplyDamageEffect(class AActor* InCauser, class AActor* InApply, FVector InDirection, float DamageValue)
{

	//if (InCauser->GetClass() == AWaterArea::StaticClass())
	//{
	//	ACharacterBase* Applier = Cast<ACharacterBase>(InApply);
	//
	//	if (Applier)
	//	{
	//		Applier->GetStateComponent()->AddHealth(-DamageValue);
	//	}
	//}
}