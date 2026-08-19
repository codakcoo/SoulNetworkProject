// Fill out your copyright notice in the Description page of Project Settings.


#include "EQS/FriendlyQueryContext.h"
#include "Classes/AIControllerBase.h"
#include "Classes/AIManager.h"
#include "Classes/CharacterBase.h"
#include "Classes/EnemyCharacter.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UFriendlyQueryContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AAIControllerBase* Con = Cast<AAIControllerBase>((Cast<AActor>((QueryInstance.Owner).Get())->GetInstigatorController()));
	if (Con && Con->Agent)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, Con->Agent);
		//AActor* FriendActor = Cast<AActor>(Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor));
		//if (Target)
		//{
		//	UEnvQueryItemType_Actor::SetContextHelper(ContextData, Target);
		//}

	}
}