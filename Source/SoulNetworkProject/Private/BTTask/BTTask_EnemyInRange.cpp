// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_EnemyInRange.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"


#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SoulNetworkLog.h"

UBTTask_EnemyInRange::UBTTask_EnemyInRange(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Enemy In Range";
	BlackboardKey.AddObjectFilter(this, *NodeName, AActor::StaticClass());

	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	TraceCannels.Add(WorldStatic);
}

EBTNodeResult::Type UBTTask_EnemyInRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	Chr = Cast<ACharacterBase>(Con->GetPawn());

	if (Con && Chr)
	{
		if (SphereTrace())
		{
			if (!Chr->InputType.InputCheckBoolean())
			{
				Chr->InputType.bButtonWhenRoll = bDodge;
				Chr->InputType.bButtonWhenAttack = bAttack;

				UE_LOG(LogSoulAI, Warning, TEXT("Call InRange"));

				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}

bool UBTTask_EnemyInRange::SphereTrace()
{
	bool bOverlap = false;

	FHitResult HitResult;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Chr);

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Chr->GetActorLocation(), Radius, TraceCannels, ACharacterBase::StaticClass(), IgnoreActors, OutActors);
	if(!bHit) bOverlap = false;
	else
	{
		for (auto& HitActor : OutActors)
		{
			if (Cast<APlayerCharacter>(HitActor))
			{
				bOverlap = true;
				break;
			}
		}
	}

	return bOverlap;
}
