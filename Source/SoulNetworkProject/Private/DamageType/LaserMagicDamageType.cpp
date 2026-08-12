// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageType/LaserMagicDamageType.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"
#include "SoulNetworkLog.h"

void ULaserMagicDamageType::ApplyDamageEffect(AActor* InCauser, AActor* InApply, FVector InDirection, float DamageValue)
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
					Applier->GetCombatComponent()->RunGuardMotion(EAttackType::Low);
					//Applier->GetCombatComponent()->RunHurtMotion();


					float Damage = (-GetTotalDamageValue(Causer, Applier, DamageValue) / (60.0f / DamagePossibleDelay)) / 3.f;
					UE_LOG(LogSoulCombat, Warning, TEXT("Total Damage : %f"), Damage);


					Applier->GetStateComponent()->AddStamina(-10.f);
					Applier->GetStateComponent()->AddHealth(Damage);
					if (Applier->GetStateComponent()->GetCurrentStamina() <= 0.f)
					{
						Applier->GetCombatComponent()->Server_RunStun();
					}
				}
				else if (Applier->GetCombatComponent()->GetParrying())
				{
					Causer->GetCombatComponent()->Server_RunStun();

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

void ULaserMagicDamageType::BasicDamageEvent(ACharacterBase* InCauser, ACharacterBase* InApply, FVector InDirection, float DamageValue)
{
	if (InApply->GetStateComponent()->GetUseHyperArmor())
	{
		InApply->GetStateComponent()->AddCurrentPoiseHealth(-InApply->GetStateComponent()->GetCurrentPoiseDamage());
		if (InApply->GetStateComponent()->GetCurrentPoiseHealth() <= 0.f)
		{
			if (InApply->GetStateComponent()->GetUseRigidity())
			{
				//Server_Hurt(HurtMt, 0);
				InApply->GetCombatComponent()->RunHurtMotion(InCauser->GetActorRotation());
				//InApply->GetCombatComponent()->Server_HurtMotion();

			}
		}
	}
	else
	{
		if (InApply->GetStateComponent()->GetUseRigidity())
		{
			//Server_Hurt(HurtMt, 0);

			InApply->GetCombatComponent()->RunHurtMotion(InCauser->GetActorRotation());
			//InApply->GetCombatComponent()->Server_HurtMotion();
		}
	}
	float Damage = -GetTotalDamageValue(InCauser, InApply, DamageValue) / (60.0f / DamagePossibleDelay);
	UE_LOG(LogSoulCombat, Warning, TEXT("Total Damage : %f"), Damage);

	InApply->GetStateComponent()->AddHealth(Damage);
	InCauser->GetEffectComponent()->EffectSequence(EEffectType::Damaged, InDirection);
	if (InCauser->GetStateComponent()->GetEnchant().IsSet()) InApply->GetEffectComponent()->Multicast_BodyMeshNigara(InCauser->GetStateComponent()->GetEnchant().ElementType, 2.f);

	if (InApply->GetCharacterMovement()->MovementMode != MOVE_Walking) InApply->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
