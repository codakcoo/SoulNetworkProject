// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_SkillOverride.h"
#include "Classes/AIControllerBase.h"
#include "Classes/BossCharacter.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "SoulNetworkLog.h"

UBTTask_SkillOverride::UBTTask_SkillOverride(const FObjectInitializer& ObjectInitialzier) : Super(ObjectInitialzier)
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
	NodeName = L"Skill Override";
}

EBTNodeResult::Type UBTTask_SkillOverride::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	ABossCharacter* Chr = Cast<ABossCharacter>(Con->GetPawn());

	if(!Con || !Chr) return EBTNodeResult::Failed;
	if(!Chr->GetCanUseSkillTree(SkillIndex)) return EBTNodeResult::Failed;

	if (!Con->GetBlackboard()->GetValueAsBool(bb_keys::montage))
	{
		if (Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor))
		{
			Chr->Server_SkillMontage(SkillIndex);
			//Con->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
			//Con->GetBlackboard()->SetValueAsBool(bb_keys::montage, true);

			Con->ClearAttackTimer();
		}
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_SkillOverride::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);


	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	ABossCharacter* Chr = Cast<ABossCharacter>(Con->GetPawn());
	if (Con && Chr)
	{
		if (Con->GetBlackboard()->GetValueAsBool(bb_keys::montage))
		{
			if (!Chr->AnimValues.ValueCheckBoolean())
			{
				UE_LOG(LogSoulAI, Warning, TEXT("Successed"));
				Con->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
	}
}
