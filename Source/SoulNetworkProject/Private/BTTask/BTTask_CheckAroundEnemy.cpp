// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_CheckAroundEnemy.h"
#include "Classes/CharacterBase.h"
#include "Classes/BossCharacter.h"
#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"
#include "SoulNetworkProject/CharacterState.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "SoulNetworkDebug.h"

UBTTask_CheckAroundEnemy::UBTTask_CheckAroundEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = L"Check Around Enemy";

	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);
}

EBTNodeResult::Type UBTTask_CheckAroundEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	ACharacterBase* Chr = Cast<ACharacterBase>(Con->GetPawn());
	if(!Con || !Chr) return EBTNodeResult::Failed;

	TArray<AActor*> OutActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Chr);

	if (SoulDebug::IsDrawEnabled())
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Chr->GetActorLocation(), Search_Radius, 12, FLinearColor(1.f, 1.f, 1.f));
	}
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Chr->GetActorLocation(), Search_Radius, ObjectTypes, AActor::StaticClass(), IgnoreActors, OutActors);
	if (!bHit)
	{	
		Con->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
		Con->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, static_cast<uint8>(ECombatState::Strafe));
		return EBTNodeResult::Failed;
	}
	else
	{
		for (auto& HitActor : OutActors)
		{
			if (ACharacterBase* Player = Cast<ACharacterBase>(HitActor))
			{
				if (Chr->GetFaction() != Player->GetFaction())
				{
					if (ABossCharacter* Boss = Cast<ABossCharacter>(Chr))
					{
						if (Boss->HasActorInArea(Player)) EnemyActors.Add(Player);
					}
					else
					{
						EnemyActors.Add(Player);
					}
				}
			}
		}
	}

	float FirstDistance = 0.f;
	AActor* OptimalActor = nullptr;

	if(EnemyActors.Num() == 0)
	{
		//Con->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
		//Con->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, static_cast<uint8>(ECombatState::Strafe));



		return EBTNodeResult::Failed;
	}
	else
	{
		for (auto& Enemy : EnemyActors)
		{
			if(FirstDistance == 0.f)
			{
				OptimalActor = Enemy;
				FirstDistance =  (Chr->GetActorLocation() - Enemy->GetActorLocation()).Size();
			}
			else
			{
				if (FirstDistance > (Chr->GetActorLocation() - Enemy->GetActorLocation()).Size())
				{
					OptimalActor = Enemy;
					FirstDistance = (Chr->GetActorLocation() - Enemy->GetActorLocation()).Size();
				}
			}
		}
	}

	FRotator OwnerRot = Chr->GetActorRotation();
	FRotator EnemyRot = OptimalActor->GetActorRotation();
	float CurrentDeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(OwnerRot, EnemyRot).Yaw;

	Con->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, OptimalActor);

	Con->GetBlackboard()->SetValueAsBool(
		bb_keys::behind, (CurrentDeltaYaw > -90.f && CurrentDeltaYaw < -0.f) || 
						 (CurrentDeltaYaw > 0.f && CurrentDeltaYaw < 90.f) ? true :false);

	return EBTNodeResult::Succeeded;
}
