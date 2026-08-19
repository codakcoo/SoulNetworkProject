// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageType/MagicDamageType.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"

float UMagicDamageType::GetTotalDamageValue(ACharacterBase* InCauser, ACharacterBase* InApply, float BaseDamageValue)
{
	if(!InCauser || !InApply || !InCauser->GetStateComponent() || !InApply->GetStateComponent()) return 0.f;

	float TotalDamage = 0.f;

	float CauserDamage = 0.f;
	float ApplyerBlock = 0.f;

	for (const auto& Data : InCauser->GetStateComponent()->GetEnforces())
	{
		if(Data.ElementType == SpellType && Data.IncreaseType == EBuffIncreaseType::Damage) CauserDamage += Data.BuffIncreaseValue;
	}

	for (const auto& Data : InApply->GetStateComponent()->GetEnforces())
	{
		if (Data.ElementType == SpellType && Data.IncreaseType == EBuffIncreaseType::Defense) ApplyerBlock += Data.BuffIncreaseValue;
	}


	TotalDamage = (BaseDamageValue * (InCauser->GetStateComponent()->GetINT() / InApply->GetStateComponent()->GetMR())) + (CauserDamage - ApplyerBlock > 0.f ? CauserDamage - ApplyerBlock : 0.f);

	return TotalDamage;
}