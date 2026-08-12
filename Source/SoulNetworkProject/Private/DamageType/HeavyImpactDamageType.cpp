// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageType/HeavyImpactDamageType.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/EffectComponent.h"
#include "Gameframework/CharacterMovementComponent.h"

#include "Interface/CombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

void UHeavyImpactDamageType::ApplyDamageEffect(AActor* InCauser, AActor* InApply, FVector InDirection, float DamageValue)
{
	ACharacterBase* Causer = Cast<ACharacterBase>(InCauser);
	ACharacterBase* Applier = Cast<ACharacterBase>(InApply);
	
	if (Causer && Applier)
	{
		if (!Applier->GetStateComponent()->GetDamagePossible()) return;

		if (Causer->GetCombatComponent() && Applier->GetStateComponent())
		{
			//
			Applier->GetStateComponent()->OnDamagePossible(DamagePossibleDelay);

			FRotator CurrentRot = Applier->GetActorRotation();
			FRotator CauserRot = Causer->GetActorRotation();
			float CurrentDeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRot, CauserRot).Yaw;
			if ((CurrentDeltaYaw >= -180.f && CurrentDeltaYaw < -130.f) || (CurrentDeltaYaw <= 180.f && CurrentDeltaYaw > 130.f))
			{
				if (Applier->GetCombatComponent()->GetGuardup())
				{
					//Applier->Server_Hurt(GuardMt, 0);
					Applier->GetEffectComponent()->EffectSequence(EEffectType::Block, InDirection);
					Applier->GetCombatComponent()->RunGuardMotion(EAttackType::Heavy);
					//Applier->GetCombatComponent()->RunHurtMotion();

					float Damage = -GetTotalDamageValue(Causer, Applier, DamageValue, Causer->GetHoldType());

					Applier->GetStateComponent()->AddStamina(-20.0f);
					Applier->GetStateComponent()->AddHealth(Damage / 2.f);
					if (Applier->GetStateComponent()->GetCurrentStamina() <= 0.0f)
					{
						//GEngine->AddOnScreenDebugMessage(-1,10.f,)
						Applier->GetCombatComponent()->Server_RunStun();
					}
				}
				else if (Applier->GetCombatComponent()->GetParrying())
				{
					float Damage = -GetTotalDamageValue(Causer, Applier, DamageValue, Causer->GetHoldType());

					Applier->GetEffectComponent()->EffectSequence(EEffectType::Block, InDirection);
					Applier->GetStateComponent()->AddHealth(Damage / 3.f);

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

void UHeavyImpactDamageType::BasicDamageEvent(ACharacterBase* InCauser, ACharacterBase* InApply, FVector InDirection, float DamageValue)
{
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

	float Damage = -GetTotalDamageValue(InCauser, InApply, DamageValue, InCauser->GetHoldType());
	InApply->GetStateComponent()->AddHealth(Damage);
	InCauser->GetEffectComponent()->EffectSequence(EEffectType::Damaged, InDirection);
	if (InApply->GetCharacterMovement()->MovementMode != MOVE_Walking) InApply->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
