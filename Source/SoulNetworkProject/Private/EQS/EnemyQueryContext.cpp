// Fill out your copyright notice in the Description page of Project Settings.


#include "EQS/EnemyQueryContext.h"
#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "BehaviorTree/BlackboardComponent.h"


void UEnemyQueryContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AAIControllerBase* Con = Cast<AAIControllerBase>((Cast<AActor>((QueryInstance.Owner).Get())->GetInstigatorController()));
	if (Con)
	{
		AActor* Target = Cast<AActor>(Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor));
		if (Target)
		{
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, Target);
		}
	}
}