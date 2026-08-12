// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_AnotherMontageOverride.h"
#include "Classes/CharacterBase.h"
#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "SoulNetworkLog.h"

UBTTask_AnotherMontageOverride::UBTTask_AnotherMontageOverride(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	bNotifyTick = true;

	NodeName = L"Another Montage Override";
}

EBTNodeResult::Type UBTTask_AnotherMontageOverride::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	ACharacterBase* Chr = Cast<ACharacterBase>(Con->GetPawn());

	if(!Con || !Chr) return EBTNodeResult::Failed;

	
	if (RunType == ERunMtType::Custom)
	{
		Chr->Server_InteractionMontage(CustomMt, SectionName);
	}
	else if (RunType == ERunMtType::Dodge)
	{
		Chr->Server_RollMontage(static_cast<uint8>(DodgeType));
	}
	else if (RunType == ERunMtType::Equip)
	{
		Chr->Server_WeaponChangeMontage(static_cast<uint8>(EquipType) - 1);
		//UpdatedMainWeaponMesh()
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_AnotherMontageOverride::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	ACharacterBase* Chr = Cast<ACharacterBase>(Con->GetPawn());

	if (!Con && !Chr) return;

	if (!Chr->AnimValues.ValueCheckBoolean())
	{
		UE_LOG(LogSoulAI, Warning, TEXT("Successed"));
		Con->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
