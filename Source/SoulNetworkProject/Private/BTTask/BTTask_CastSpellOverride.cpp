// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_CastSpellOverride.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"
#include "Classes/EnemyCharacter.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SoulNetworkDebug.h"

UBTTask_CastSpellOverride::UBTTask_CastSpellOverride(const FObjectInitializer& ObjectInitializer)
{
    bCreateNodeInstance = true;
	bNotifyTick = true;
    NodeName = "Cast Spell";

	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);
}

EBTNodeResult::Type UBTTask_CastSpellOverride::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	Chr = Cast<ACharacterBase>(Con->GetPawn());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Con->GetPawn());
	if (Con && Enemy)
	{
		if (!Con->GetBlackboard()->GetValueAsBool(bb_keys::montage))
		{
			if (Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor) && !Enemy->AnimValues.ValueCheckBoolean())
			{
				if(!Enemy->SetCurrentSpellCount(CastSpellCount)) return EBTNodeResult::Failed;

				Con->ClearAttackTimer();

				CurrentSpell.PermuteValue(Enemy->GetSpells()[CastSpellCount]);
				Enemy->Server_E_SpellMontage(CurrentSpell);

				if (CurrentSpell.SpellData.MotionType == ESpellMotionType::Charge)
				{
					GetWorld()->GetTimerManager().SetTimer(ChargeTimer, this, &UBTTask_CastSpellOverride::CancelChargeSpell ,10.f, false);
				}
			}
		}
	}


    return EBTNodeResult::InProgress;
}

void UBTTask_CastSpellOverride::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (Con)
	{
		if (Con->GetBlackboard()->GetValueAsBool(bb_keys::montage))
		{
			if (Chr->AnimValues.ValueCheckBoolean())
			{
				if (Chr->GetCanInputCheck())
				{
					if (CurrentSpell.SpellData.MotionType == ESpellMotionType::Combo)
					{
						if (bUseSearchRange)
						{
							Chr->InputType.bButtonWhenSpell = Chr->GetCurrentAttackCount() == 0 ? false : OverlapTarget();
						}
						else
						{
							Chr->InputType.bButtonWhenSpell = Chr->GetCurrentAttackCount() == 0 ? false : true;
						}

					}
					else
					{
						Chr->InputType.bButtonWhenSpell = false;
					}
				}
	
			}
			else if (!Chr->AnimValues.ValueCheckBoolean())
			{
				UE_LOG(LogSoulAI, Warning, TEXT("Successed"));
				Con->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
	}
}

bool UBTTask_CastSpellOverride::OverlapTarget()
{
	bool bTrigger = false;

	TArray<AActor*> OutActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Chr);

	if (SoulDebug::IsDrawEnabled())
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Chr->GetActorLocation(), SearchRadius, 12, FLinearColor(1.f, 1.f, 1.f));
	}
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Chr->GetActorLocation(), SearchRadius, ObjectTypes, AActor::StaticClass(), IgnoreActors, OutActors);
	if (!bHit) bTrigger = false;
	else
	{
		for (auto& HitActor : OutActors)
		{
			if (Cast<APlayerCharacter>(HitActor))
			{
				bTrigger = true;
				break;
			}
		}
	}

	return bTrigger;
}

void UBTTask_CastSpellOverride::CancelChargeSpell()
{
	Chr->Server_E_SpellMontage(CurrentSpell);
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimer);
}
