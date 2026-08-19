// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_SetAnimationState.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Gameframework/CharacterMovementComponent.h"

UBTTask_SetAnimationState::UBTTask_SetAnimationState(const FObjectInitializer& ObjectInitialzier) : Super(ObjectInitialzier)
{
    bCreateNodeInstance = true;
    NodeName = "Set Animation State";
    BlackboardKey.AddObjectFilter(this, *NodeName, AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_SetAnimationState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIControllerBase* Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
    ACharacterBase* Chr = Cast<ACharacterBase>(Con->GetPawn());
    if (Con && Chr)
    {
        //Chr->ToggleSpeed(GetWalkSpeed(Chr));
        Chr->GetCharacterMovement()->MaxWalkSpeed = GetWalkSpeed(Chr);
        Chr->ToggleADS(bADS);
        Chr->ToggleCombat(bCombat);
        Chr->ToggleGuard(bGuard);

        ACharacterBase* Enemy = Cast<ACharacterBase>(Con->GetBlackboard()->GetValueAsObject(bb_keys::target_actor));
        if (Enemy)
        {
            (bFocus) ? Con->SetFocus(Enemy) : Con->ClearFocus(EAIFocusPriority::LastFocusPriority);
            Chr->bUseControllerRotationYaw = bFocus ? true : false;

            if (Chr->CombatRole == ECombatRole::Range)
            {
                Chr->Server_ToggleAim(bAim);
            }

            return EBTNodeResult::Succeeded;
        }

        Con->ClearFocus(EAIFocusPriority::LastFocusPriority);
        Chr->bUseControllerRotationYaw = false;
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}

float UBTTask_SetAnimationState::GetWalkSpeed(ACharacterBase* Owner)
{
	switch (Speed)
	{
	    case ESpeedType::Walk:
            return Owner->GetWalkSpeed();
        case ESpeedType::Sprint:
            return Owner->GetSprintSpeed();
        case ESpeedType::Slow:
            return Owner->GetSlowSpeed();
	    default:
	    	return Owner->GetWalkSpeed();
	}

    return Owner->GetWalkSpeed();
}
