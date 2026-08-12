// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageType/HealMagicDamageType.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/EffectComponent.h"
#include "SoulNetworkLog.h"

void UHealMagicDamageType::ApplyDamageEffect(AActor* InCauser, AActor* InApply, FVector InDirection, float DamageValue)
{
	ACharacterBase* Causer = Cast<ACharacterBase>(InCauser);
	ACharacterBase* Applier = Cast<ACharacterBase>(InApply);

	if (Causer && Applier)
	{
		if (Causer->GetCombatComponent() && Applier->GetStateComponent() && Causer->GetFaction() == Applier->GetFaction())
		{
			float TotalDamage = 0.f;
			float CauserDamage = 0.f;
			//for (const auto& Data : Causer->GetStateComponent()->GetEnforces())
			//{
			//	if (Data.ElementType == SpellType && Data.IncreaseType == EBuffIncreaseType::Damage) CauserDamage += Data.BuffIncreaseValue;
			//}
			float Damage = DamageValue + (DamageValue * (Causer->GetStateComponent()->GetINT() / 100)) + CauserDamage;
			//float Damage = GetTotalDamageValue(Causer, Applier, DamageValue);


			UE_LOG(LogSoulCombat, Warning, TEXT("Total Damage : %f"), Damage);
			Applier->GetStateComponent()->AddHealth(Damage);
			Applier->GetEffectComponent()->EffectSequence(EEffectType::Health, FVector::ZeroVector);

		}
	}
}
