// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_AttackOverride.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"
#include "Classes/PlayerCharacter.h"

#include "SoulNetworkProject/ItemState.h"
#include "SoulNetworkProject/ItemData.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SoulNetworkDebug.h"

UBTTask_AttackOverride::UBTTask_AttackOverride(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
	NodeName = "Attack Override";

	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);
}

EBTNodeResult::Type UBTTask_AttackOverride::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);


	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	Chr = Cast<ACharacterBase>(Con->GetPawn());
	if (Con && Chr)
	{
		if (!Con->GetBlackboard()->GetValueAsBool(bb_keys::montage))
		{
			if (Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor))// && !Chr->AnimValues.ValueCheckBoolean())
			{
				if(AttackType == ERunAttackType::LowAttack)
				{
					if(!Chr->GetHoldTypeWeapon().IsSet()) Chr->Server_LowAttackMontage();
					else
					{
						if(Chr->GetHoldTypeWeapon().WeaponData.WeaponType == EWeaponType::Bow) Chr->Server_BowMontage(false);
						else Chr->Server_E_LowAttackMontage();
					}
				}
				else if(AttackType == ERunAttackType::HeavyAttack)
				{
					if (Chr->GetHoldTypeWeapon().IsSet() && 
						Chr->GetHoldTypeWeapon().WeaponData.WeaponType == EWeaponType::Bow) Chr->Server_BowMontage(true);
					else
					{
						Chr->Server_HeavyAttackMontage();

					}
				}
				else if(AttackType == ERunAttackType::AOEAttack)
				{
					Chr->GetHoldTypeWeapon().IsSet() ? Chr->Server_E_AOEAttackMontage() :Chr->Server_AOEAttackMontage();
				}

				Con->ClearAttackTimer();

			}
		}
	}	

	return EBTNodeResult::InProgress;
}

void UBTTask_AttackOverride::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	//if (Chr->AnimValues.ValueCheckBoolean()) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if(Con)
	{
		if (Con->GetBlackboard()->GetValueAsBool(bb_keys::montage))
		{
			if (Chr->AnimValues.ValueCheckBoolean())
			{
				if (Chr->GetCanInputCheck())
				{
					if (AttackType == ERunAttackType::LowAttack)
					{
						if (Chr->GetCurrentAttackCount() == 0)
						{
							UE_LOG(LogSoulAI, Warning, TEXT("Attack End"));
							Con->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);
							FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

							return;
						}

						Chr->InputType.bButtonWhenAttack = bUseSearchRange ? OverlapTarget() : true;
					}
					else if (AttackType == ERunAttackType::HeavyAttack)
					{
						if (Con->GetBlackboard()->GetValueAsBool(bb_keys::damaged))
						{
							Con->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
							Chr->Server_HeavyAttackMontage();
						}
					}
					else if (AttackType == ERunAttackType::AOEAttack)
					{
						if (Chr->CurrentAOECount == 0)
						{
							UE_LOG(LogSoulAI, Warning, TEXT("Attack End"));
							Con->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);
							FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

							return;
						}
						Chr->InputType.bButtonWhenAOE = bUseSearchRange ? OverlapTarget() : true;
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

bool UBTTask_AttackOverride::OverlapTarget()
{
	bool bTrigger = false;

	TArray<AActor*> OutActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Chr);

	if (SoulDebug::IsDrawEnabled())
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Chr->GetActorLocation(), Search_Radius, 12, FLinearColor(1.f, 1.f, 1.f), 5.0f);
	}
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Chr->GetActorLocation(), Search_Radius, ObjectTypes, AActor::StaticClass(), IgnoreActors, OutActors);
	if(!bHit)
	{
		bTrigger = false;
	}
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
