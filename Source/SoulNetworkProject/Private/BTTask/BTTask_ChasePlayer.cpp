// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_ChasePlayer.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Chase Target";

	BlackboardKey.AddObjectFilter(this, *NodeName, AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	ACharacterBase* Target = Cast<ACharacterBase>(Con->GetBlackboard()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if(!Con || !Target) EBTNodeResult::Failed;

	const FVector TargetLoc = Target->GetActorLocation();
	if (Search_Random)
	{
		FNavLocation NavLoc;

		//�׺���̼� �ý������� �÷��̾ ã�������� ���� ��ġ�� ������//
		UNavigationSystemV1* const Nav_System = UNavigationSystemV1::GetCurrent(GetWorld());
		if (Nav_System->GetRandomPointInNavigableRadius(TargetLoc, Search_Radius, NavLoc, nullptr))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Con, NavLoc.Location);
		}
	}
	else
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Con, TargetLoc);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
