// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageType/RadialDamageType.h"
#include "Classes/CharacterBase.h"
#include "Classes/EffectComponent.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/CombatComponent.h"
#include "Interface/CombatInterface.h"

#include "Kismet/KismetMathLibrary.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "SoulNetworkDebug.h"


void URadialDamageType::ApplyDamageEffect(AActor* InCauser, AActor* InApply, FVector InDirection, float DamageValue)
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
			FRotator FindLookRotation = UKismetMathLibrary::FindLookAtRotation(InApply->GetActorLocation(), InDirection);
			//float CurrentDeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRot, FindLookRotation).Yaw;
			float CurrentDeltaYaw = FMath::Abs(CurrentRot.Yaw - FindLookRotation.Yaw);

			SoulDebug::ScreenLog(FString::Printf(TEXT("%f"), CurrentDeltaYaw), FColor::Purple, 10.f);
			if ((CurrentDeltaYaw >= 0.0f && CurrentDeltaYaw < 50.0f))
			{
				if (Applier->GetCombatComponent()->GetGuardup())
				{
					//Applier->Server_Hurt(GuardMt, 0);
					Applier->GetEffectComponent()->EffectSequence(EEffectType::Block, InDirection);
					Applier->GetCombatComponent()->RunGuardMotion(StrongType == EStrongType::Low ? EAttackType::Low : EAttackType::Heavy);
					//Applier->GetCombatComponent()->RunHurtMotion();
			
			
					float Damage = -GetTotalDamageValue(Causer, Applier, DamageValue) / 3.f;
					UE_LOG(LogSoulCombat, Warning, TEXT("Total Damage : %f"), Damage);
			
			
					Applier->GetStateComponent()->AddStamina(-10.f);
					Applier->GetStateComponent()->AddHealth(Damage);
					if (Applier->GetStateComponent()->GetCurrentStamina() <= 0.f)
					{
						Applier->GetCombatComponent()->Server_RunStun();
					}
				}
				//else if (Applier->GetCombatComponent()->GetParrying())
				//{
				//	Causer->GetCombatComponent()->Server_RunStun();
				//
				//}
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

void URadialDamageType::BasicDamageEvent(ACharacterBase* InCauser, ACharacterBase* InApply, FVector InDirection, float DamageValue)
{
	if (InApply->GetStateComponent()->GetUseHyperArmor())
	{
		InApply->GetStateComponent()->AddCurrentPoiseHealth(-InApply->GetStateComponent()->GetCurrentPoiseDamage());
		if (InApply->GetStateComponent()->GetCurrentPoiseHealth() <= 0.f)
		{
			if (InApply->GetStateComponent()->GetUseRigidity())
			{
				//Server_Hurt(HurtMt, 0);
				if(StrongType == EStrongType::Low) InApply->GetCombatComponent()->RunHurtMotion(InCauser->GetActorRotation());
				else if (StrongType == EStrongType::Heavy) InApply->GetCombatComponent()->RunHurtMotion(InDirection);
				//InApply->GetCombatComponent()->Server_HurtMotion();

			}
		}
	}
	else
	{
		if (InApply->GetStateComponent()->GetUseRigidity())
		{	
			if (StrongType == EStrongType::Low) InApply->GetCombatComponent()->RunHurtMotion(InCauser->GetActorRotation());
			else if (StrongType == EStrongType::Heavy) InApply->GetCombatComponent()->RunHurtMotion(InDirection);
			//InApply->GetCombatComponent()->Server_HurtMotion();
		}
	}
	float Damage = -GetTotalDamageValue(InCauser, InApply, DamageValue);
	UE_LOG(LogSoulCombat, Warning, TEXT("Total Damage : %f"), Damage);

	InApply->GetStateComponent()->AddHealth(Damage);
	InCauser->GetEffectComponent()->EffectSequence(EEffectType::Damaged, InDirection);
	if (InCauser->GetStateComponent()->GetEnchant().IsSet()) InApply->GetEffectComponent()->Multicast_BodyMeshNigara(InCauser->GetStateComponent()->GetEnchant().ElementType, 2.f);

	if (InApply->GetCharacterMovement()->MovementMode != MOVE_Walking) InApply->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}