// Fill out your copyright notice in the Description page of Project Settings.


#include "Service/BTService_AllStopMontage.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"

#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_AllStopMontage::UBTService_AllStopMontage(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "All Stop Montage";
}

void UBTService_AllStopMontage::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIControllerBase* AI = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	ACharacterBase* Agent = Cast<ACharacterBase>(AI->GetPawn());
	if(!AI || !Agent) return;

	if (AI->GetBlackboard()->GetValueAsBool(bb_keys::montage) || Agent->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		Agent->GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
		AI->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);
	}
}
