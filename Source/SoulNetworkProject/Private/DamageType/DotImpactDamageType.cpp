// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageType/DotImpactDamageType.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/EffectComponent.h"
#include "Gameframework/CharacterMovementComponent.h"

#include "Interface/CombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

void UDotImpactDamageType::ApplyDamageEffect(AActor* InCauser, AActor* InApply, FVector InDirection, float DamageValue)
{
	ACharacterBase* Causer = Cast<ACharacterBase>(InCauser);
	ACharacterBase* Applier = Cast<ACharacterBase>(InApply);

	if (Causer && Applier)
	{
		if (!Applier->GetStateComponent()->GetDamagePossible()) return;

		if (Causer->GetCombatComponent() && Applier->GetStateComponent())
		{
			Applier->GetStateComponent()->OnDamagePossible(DamagePossibleDelay);

			FRotator CurrentRot = Applier->GetActorRotation();
			FRotator CauserRot = Causer->GetActorRotation();
			float CurrentDeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRot, CauserRot).Yaw;
			if ((CurrentDeltaYaw >= -180.f && CurrentDeltaYaw < -130.f) || (CurrentDeltaYaw <= 180.f && CurrentDeltaYaw > 130.f))
			{
				if (Applier->GetCombatComponent()->GetGuardup() || Applier->GetCombatComponent()->GetParrying())
				{
					//Applier->Server_Hurt(GuardMt, 0);
					Applier->GetEffectComponent()->EffectSequence(EEffectType::Block, InDirection);
					Applier->GetCombatComponent()->RunGuardMotion(EAttackType::Low);
					//Applier->GetCombatComponent()->RunHurtMotion();


					Applier->GetStateComponent()->AddStamina(-10.f);
					if (Applier->GetStateComponent()->GetCurrentStamina() <= 0.f)
					{
						Applier->GetCombatComponent()->Server_RunStun();
					}
				}
				else
				{
					BasicDamageEvent(Causer, Applier, InDirection, DamageValue);
				}
			}
			else
			{
				BasicDamageEvent(Causer, Applier, InDirection, DamageValue);
			}
		}

		if (Applier->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			Cast<ICombatInterface>(Applier)->UpdatePointDamage(Causer);
		}
	}
}

void UDotImpactDamageType::BasicDamageEvent(ACharacterBase* InCauser, ACharacterBase* InApply, FVector InDirection, float DamaValue)
{
	if(!InCauser || !InApply || !InApply->GetStateComponent() || !InApply->GetCombatComponent()) return;

	int8 Current_Indedx = 0;// = FMath::RandRange(0, InApply->GetCombatComponent()->GetMaxDotHurtCount());

	if (InApply->GetStateComponent()->GetUseHyperArmor())
	{
		InApply->GetStateComponent()->AddCurrentPoiseHealth(-InApply->GetStateComponent()->GetCurrentPoiseDamage());
		if (InApply->GetStateComponent()->GetCurrentPoiseHealth() <= 0.f)
		{
			if (InApply->GetStateComponent()->GetUseRigidity())
			{
				//Server_Hurt(HurtMt, 0);

				InApply->GetCombatComponent()->RunHurtMotion();
				//InApply->GetCombatComponent()->Server_HurtMotion();

			}
		}
	}
	else
	{
		if (InApply->GetStateComponent()->GetUseRigidity())
		{
			//Server_Hurt(HurtMt, 0);

			InApply->GetCombatComponent()->RunHurtMotion();
			//InApply->GetCombatComponent()->Server_HurtMotion();
		}
	}

	InApply->GetStateComponent()->AddHealth(-10.f);
	InCauser->GetEffectComponent()->EffectSequence(EEffectType::Damaged, InDirection);
	if (InApply->GetCharacterMovement()->MovementMode != MOVE_Walking) InApply->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

}
