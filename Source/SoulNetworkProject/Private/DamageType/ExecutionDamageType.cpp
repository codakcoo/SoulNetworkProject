// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageType/ExecutionDamageType.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/EffectComponent.h"

#include "Interface/CombatInterface.h"


void UExecutionDamageType::ApplyDamageEffect(class AActor* InCauser, class AActor* InApply, FVector InDirection, float DamageValue)
{
	ACharacterBase* Causer = Cast<ACharacterBase>(InCauser);
	ACharacterBase* Applier = Cast<ACharacterBase>(InApply);

	if (Causer && Applier)
	{
		if (Causer->GetCombatComponent() && Applier->GetStateComponent())
		{

			float Damage = -GetTotalDamageValue(Causer, Applier, DamageValue, EEquipHand::None);
			if(!Causer->GetMainWeapon().IsSet()) return;
			Damage *= 100.0f/100.0f;

			Applier->GetStateComponent()->AddHealth(Damage);
			//Causer->GetEffectComponent()->EffectSequence(EEffectType::Damaged, InDirection);
			if (Causer->GetStateComponent()->GetEnchant().IsSet()) Applier->GetEffectComponent()->Multicast_BodyMeshNigara(Causer->GetStateComponent()->GetEnchant().ElementType, 2.f);
		}

		if (Applier->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			Cast<ICombatInterface>(Applier)->UpdatedExecutionDamage(Causer);
		}
	}
}
