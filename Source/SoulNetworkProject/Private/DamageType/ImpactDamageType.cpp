// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageType/ImpactDamageType.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"
#include "SoulNetworkLog.h"

float UImpactDamageType::GetTotalDamageValue(class ACharacterBase* InCauser, class ACharacterBase* InApply, float BaseDamageValue, EEquipHand HandType)
{
	if (!InCauser || !InApply || !InCauser->GetStateComponent() || !InApply->GetStateComponent()) return 0.f;

	float TotalDamage = 0.f;

	float CauserDamage = 0.f;
	float ApplyerBlock = 0.f;

	if (InCauser->GetStateComponent()->GetEnchant().IsSet())
	{
		EBuffElementType CurrentElementType = InCauser->GetStateComponent()->GetEnchant().ElementType;
		CauserDamage += InCauser->GetStateComponent()->GetEnchant().BuffIncreaseValue;
		for (const auto& Data : InCauser->GetStateComponent()->GetEnforces())
		{
			if (Data.ElementType == EBuffElementType::STR) continue;
			if (Data.ElementType == CurrentElementType && Data.IncreaseType == EBuffIncreaseType::Damage) CauserDamage += Data.BuffIncreaseValue;
		}

		for (const auto& Data : InApply->GetStateComponent()->GetEnforces())
		{
			if (Data.ElementType == EBuffElementType::STR) continue;
			if (Data.ElementType == CurrentElementType && Data.IncreaseType == EBuffIncreaseType::Defense) ApplyerBlock += Data.BuffIncreaseValue;
		}
	}

	float TotalSTR = 0.f;
	float TotalDEF = 0.f;
	float MotionValue = 1.0f;

	if (InCauser->AnimValues.bAirAttack)
	{
		MotionValue += 0.1f;
	}
	else if (InCauser->AnimValues.bHeavyAttack)
	{
		MotionValue += 0.2f;
	}
	else if (InCauser->AnimValues.bAOEAttack)
	{
		MotionValue += 0.3f;
	}

	if (HandType != EEquipHand::None)
	{
		MotionValue += 0.5f;
	}
	
	UE_LOG(LogSoulCombat, Warning, TEXT("MotionValue %f"), MotionValue);

	for (const auto& Data : InCauser->GetStateComponent()->GetEnforces())
	{
		if (Data.ElementType == EBuffElementType::STR && Data.IncreaseType == EBuffIncreaseType::Damage) TotalSTR += Data.BuffIncreaseValue;
	}

	for (const auto& Data : InApply->GetStateComponent()->GetEnforces())
	{
		if (Data.ElementType == EBuffElementType::STR && Data.IncreaseType == EBuffIncreaseType::Defense) TotalDEF += Data.BuffIncreaseValue;
	}


	TotalSTR += InCauser->GetStateComponent()->GetSTR() + BaseDamageValue;
	TotalDEF += InApply->GetStateComponent()->GetDEF();

	TotalDamage = (TotalSTR - TotalDEF) * MotionValue + (CauserDamage - ApplyerBlock > 0.f ? CauserDamage - ApplyerBlock : 0.f);

	return TotalDamage;
}