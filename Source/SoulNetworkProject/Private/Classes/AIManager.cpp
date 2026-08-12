// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/AIManager.h"
#include "Classes/AIControllerBase.h"
#include "Classes/EnemyCharacter.h"
#include "Classes/PlayerCharacter.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AAIManager::AAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();

	if(Agents.Num() == 0) return;

	CreateControlList();
	//for (auto& Agent : Agents)
	//{
	//	Agent->SetupCharacter(!Agent->GetDead());
	//}
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AAIManager::Engaged()
{
	bool ED = false;

	for (auto& Con : Controls)
	{
		if (Con)
		{
			if (Con->GetBlackboard()->GetValueAsBool(bb_keys::can_see_player))
			{
				ED = true;
				break;
			}

			if (Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor))
			{
				ED = true;
				break;
			}


		}
	}
	return false;
}

void AAIManager::NotifyAIState(EAIState State)
{
	for (AAIControllerBase* Con : Controls)
	{
		Engaged() ?  Con->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Attack) : Con->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)State);
		Con->BBC->SetValueAsVector(bb_keys::last_Location, LastStimulusLocation);
	}

	if (State == EAIState::Attack)
	{
		SetTargetAllAgent();
		//GetWorldTimerManager().ClearTimer(SearchTimer);
		//GetWorldTimerManager().SetTimer(CombatTimer, this, )
	}
	//
	//if (State == EAIState::ManDown)
	//{
	//	GetWorldTimerManager().SetTimer(MandownTimer, this, &AAIManager::)
	//}
}

bool AAIManager::GetCanAttack()
{
	uint8 CurrentAttackCount = 0;

	for (auto& Con : Controls)
	{
		if (Con && Con->Agent && Con->Agent->CombatRole != ECombatRole::Range)
		{
			if (Con->GetBlackboard()->GetValueAsEnum(bb_keys::combat_state) == (uint8)ECombatState::Attack)
			{
				CurrentAttackCount++;
				if (CurrentAttackCount >= MaxAttackCount) return false;
			}
		}
	}

	return true;
}

bool AAIManager::GetCanFollow()
{
	uint8 CurrentFollowCount = 0;

	for (auto& Con : Controls)
	{
		if (Con && Con->Agent && Con->Agent->CombatRole != ECombatRole::Range)
		{
			if (Con->GetBlackboard()->GetValueAsEnum(bb_keys::combat_state) == (uint8)ECombatState::Follow)
			{
				CurrentFollowCount++;
				if (CurrentFollowCount >= MaxFollowCount) return false;
			}
		}
	}

	return true;
}

void AAIManager::SetupAgents(TArray<bool> Activates)
{
	if(GetAgents().Num() != Activates.Num()) return;
	for (int8 i = 0; i < Activates.Num(); i++)
	{
		if (GetAgents()[i])
		{
			if (AEnemyCharacter* EC = Cast<AEnemyCharacter>(GetAgents()[i]))
			{
				EC->SetupCharacter(Activates[i]);
			}
		}
	}
}

void AAIManager::RefreshAgents()
{
	for (auto& Con : Controls)
	{
		Con->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)ECombatState::Strafe);
		Con->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, nullptr);
	}
	for (auto& Agent : Agents)
	{
		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Agent))
		{
			Enemy->RefreshCondition();
		}
	}
}

void AAIManager::AllTransFalseforOnlyDeathValueInAgents()
{
	for (auto& Agent : Agents)
	{
		Agent->SetDead(false);
	}
}

bool AAIManager::UpdatedAttackInManager(ACharacterBase* ApplyPawn)
{
	if (ApplyPawn)
	{
		if (GetCanAttack() || ApplyPawn->CombatRole == ECombatRole::Range)
		{
			ApplyPawn->GetControllerBase()->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)ECombatState::Attack);
			ApplyPawn->GetControllerBase()->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
			ApplyPawn->GetControllerBase()->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);

			return true;
		}
	}
	return false;
}

void AAIManager::CreateControlList()
{
	for (auto& Agent : Agents)
	{
		AAIControllerBase* Con = Cast<AAIControllerBase>(Agent->GetController());
		if (Con)
		{
			Controls.AddUnique(Con);
			Con->Manager = this;
		}
	}
}

void AAIManager::SetTargetAllAgent()
{
	AActor* Target = nullptr;
	for (auto& Con : Controls)
	{
		if (Cast<APlayerCharacter>(Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor)))
		{
			Target = Cast<AActor>(Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor));
			break;
		}
	}

	if (Target)
	{
		for (auto& Con : Controls)
		{
			Con->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, Target);
		}
	}
}

void AAIManager::RunCombatLoop()
{
	if (Engaged())
	{
		
	}
}

void AAIManager::RunSearchLoop()
{
}

void AAIManager::RunMandownLoop()
{
	
}

bool AAIManager::GetAnyoneFollow()
{
	//for (auto& Con : Controls)
	//{
	//	if (Cast<APlayerCharacter>(Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor)))
	//	{
	//		Target = Cast<AActor>(Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor));
	//		break;
	//	}
	//}
	//
	//if (Target)
	//{
	//	for (auto& Con : Controls)
	//	{
	//		Con->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, Target);
	//	}
	//}

	return true;
}

