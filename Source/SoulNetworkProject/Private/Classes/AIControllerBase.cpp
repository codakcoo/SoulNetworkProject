// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/AIControllerBase.h"
#include "SoulNetworkProject/Blackboard_Keys.h"
#include "Classes/AIManager.h"
#include "Classes/CharacterBase.h"
#include "Classes/EnemyCharacter.h"
#include "Classes/CombatComponent.h"
#include "Classes/SmartObject.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#define NOISELEVELONE TEXT("Noise_Level1")
#define NOISELEVELTWO TEXT("Noise_Level2")
#define NOISELEVELTHREE TEXT("Noise_Level3")

AAIControllerBase::AAIControllerBase(const FObjectInitializer& Object_Initializer) : Super(Object_Initializer)
{
	PrimaryActorTick.bCanEverTick = true;

	BTC = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTC"));
	BBC = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBC"));

	Setup_Perception_System();
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (!Agent)
	{
		AEnemyCharacter* Chr = Cast<AEnemyCharacter>(GetPawn());
		if (!Chr) return;
		Agent = Chr;
		Agent->ControllerRef = this;
		GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)Agent->GetStartAIState());
	}

	if (Agent->SmartObject)
	{
		FGameplayTag SubTag;
		BTC->SetDynamicSubtree(SubTag, Agent->SmartObject->SubTree);
	}
	BBC->SetValueAsEnum(bb_keys::combat_role, (uint8)Agent->CombatRole);
	//BBC->SetValueAsBool(bb_keys::damaged, true);
	BBC->SetValueAsEnum(bb_keys::combat_state, (uint8)ECombatState::Strafe);
}

void AAIControllerBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AAIControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Agent->GetDead()) return;

	if (Agent && Agent->GetCombatComponent())
	{
		if (Agent->GetCombatComponent()->GetOutofControl())
		{
			BBC->SetValueAsEnum(bb_keys::combat_state, (uint8)ECombatState::Stun);

			return;
		}
	}

	if (ACharacterBase* Enemy = Cast<ACharacterBase>(BBC->GetValueAsObject(bb_keys::target_actor)))
	{
		if (Agent->AnimValues.bLowAttack || Agent->AnimValues.bHeavyAttack)
		{
			Agent->RunInterpInputVector(GetEnemyRotation(Enemy));
			
		}
	}
}

void AAIControllerBase::UpdatedAttackTimer(float Min, float Max)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(AttackTimer)) return;

	float Delay = FMath::RandRange(Min, Max);
	
	GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AAIControllerBase::CallAttackTimer, Delay, true);
}

void AAIControllerBase::ClearAttackTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
	BBC->SetValueAsBool(bb_keys::damaged, false);
	BBC->SetValueAsBool(bb_keys::montage, true);
	//BBC->SetValueAsBool(bb_keys::montage, false);
}

bool AAIControllerBase::GetHasEnemysInSight()
{
	if(!Agent || !Sight_Config || !BBC->GetValueAsBool(bb_keys::can_see_player)) return false;

	TArray<AActor*> InSightActors;
	GetAIPerceptionComponent()->GetCurrentlyPerceivedActors(Sight_Config->GetClass(), InSightActors);
	if(InSightActors.Num() == 0) return false;

	for (auto& InSightActor : InSightActors)
	{
		if (ACharacterBase* Chr = Cast<ACharacterBase>(InSightActor))
		{
			if(Chr->GetFaction() !=	Agent->GetFaction()) return true;
		}
	}

	return false;
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemyCharacter* Chr = Cast<AEnemyCharacter>(InPawn);
	if (Chr && Chr->TreeAsset)
	{
		Agent = Chr;
		
		BBC->InitializeBlackboard(*Chr->TreeAsset->BlackboardAsset);

		BTC->StartTree(*Chr->TreeAsset);

		Agent = Chr;
		Agent->ControllerRef = this;
		GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)Agent->GetStartAIState());
	}
}

UBlackboardComponent* AAIControllerBase::GetBlackboard() const
{
	return Blackboard;
}

void AAIControllerBase::OnPerception(AActor* Actor, FAIStimulus Stimuls)
{
	if (UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimuls) == UAISense_Sight::StaticClass())
	{

		if (UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimuls))
		{
			ACharacterBase* Chr = Cast<ACharacterBase>(Actor);
			if (Chr && Chr->IsHostile(Agent))
			{
				BBC->SetValueAsBool(bb_keys::can_see_player, Stimuls.WasSuccessfullySensed());

				if (BBC->GetValueAsEnum(bb_keys::ai_status) == (uint8)EAIState::Attack && !BBC->GetValueAsObject(bb_keys::target_actor))
				{
					//Agent->GetCharacterMovement()->StopActiveMovement();
					BBC->SetValueAsObject(bb_keys::target_actor, Chr);
				}
				else
				{
					BBC->SetValueAsVector(bb_keys::last_Location, Stimuls.StimulusLocation);
					BBC->SetValueAsObject(bb_keys::target_actor, Chr);
					Agent->SetLookRotation(Stimuls.StimulusLocation);
				}
			}
			return;
		}
	}
	else if (UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimuls) == UAISense_Hearing::StaticClass())
	{
		if (Stimuls.Tag == NOISELEVELONE)
		{
			if (BBC->GetValueAsEnum(bb_keys::ai_status) != (uint8)EAIState::Attack)
			{
				BBC->SetValueAsVector(bb_keys::last_Location, Stimuls.StimulusLocation);
				BBC->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Investigate);
				Agent->SetLookRotation(Stimuls.StimulusLocation);
			}
			// Blackboard() ��� ����.
		}
		else if (Stimuls.Tag == NOISELEVELTWO)
		{
			if (BBC->GetValueAsEnum(bb_keys::ai_status) != (uint8)EAIState::Attack)
			{
				BBC->SetValueAsVector(bb_keys::last_Location, Stimuls.StimulusLocation);
				BBC->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Search);
				Agent->SetLookRotation(Stimuls.StimulusLocation);
			}
		}
		else if (Stimuls.Tag == NOISELEVELTHREE)
		{
			if (ACharacterBase* Chr = Cast<ACharacterBase>(Actor))
			{
				if (Agent->GetFaction() == Chr->GetFaction()) return;

				if (BBC->GetValueAsEnum(bb_keys::ai_status) != (uint8)EAIState::Attack)
				{
					BBC->SetValueAsVector(bb_keys::last_Location, Stimuls.StimulusLocation);
					BBC->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Attack);
					BBC->SetValueAsObject(bb_keys::target_actor, Chr);
					Agent->SetLookRotation(Stimuls.StimulusLocation);
				}
			}
		}
	}
}

void AAIControllerBase::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	// �ֺ� ������ ������ �ش� ������ �±׸��� �ൿ�� ������. //
	for (size_t i = 0; i < UpdatedActors.Num(); ++i)
	{
		FActorPerceptionBlueprintInfo Info;
		GetAIPerceptionComponent()->GetActorsPerception(UpdatedActors[i], Info);
		for (size_t j = 0; j < Info.LastSensedStimuli.Num(); ++j)
		{
			const FAIStimulus Stimul = Info.LastSensedStimuli[j];
			if (Stimul.Tag == TEXT("Noise"))
			{
				if(BBC->GetValueAsEnum(bb_keys::ai_status) == (uint8)EAIState::Idle)
				{
					BBC->SetValueAsVector(bb_keys::last_Location, Stimul.StimulusLocation);
					BBC->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Investigate);
					Agent->SetLookRotation(Stimul.StimulusLocation);
				}
				//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::White,__FUNCTION__);
				// Blackboard() ��� ����.
			}
			else
			{
				// Blackboard() ��� ����.
			}
		}
	}
}

void AAIControllerBase::UpdateDetectedLevel()
{
	
}

FRotator AAIControllerBase::GetEnemyRotation(ACharacterBase* Enemy)
{
	FRotator NewRot = FRotator();
	
	if (Enemy)
	{
		NewRot = UKismetMathLibrary::FindLookAtRotation(Agent->GetActorLocation(), Enemy->GetActorLocation());
	}

	return NewRot;
}

void AAIControllerBase::UpdatedCombatBehavior(float InDeltaTime)
{
	if(BBC->GetValueAsEnum(bb_keys::ai_status) != (uint8)EAIState::Attack) return;

	if (BBC->GetValueAsEnum(bb_keys::combat_state) == (uint8)ECombatState::Strafe)
	{
		if(Agent->CombatRole == ECombatRole::Melee)
		{
			//if(Agent->GetStrafeType() == EStrafeType::Stay) return;
			//else if (Agent->GetStrafeType() == EStrafeType::Horizontal) Agent->MoveRight(StrafeDirection);
			//else if (Agent->GetStrafeType() == EStrafeType::Close) Agent->MoveForward(+1.0f);
			//Agent->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
}

void AAIControllerBase::CallAttackTimer()
{
	//if (!Manager) GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
	//else if (Manager->GetCanUseAttack() || Agent->CombatRole == ECombatRole::Range)
	//{
	//	BBC->SetValueAsEnum(bb_keys::combat_state, (uint8)ECombatState::Attack);
	//	BBC->SetValueAsBool(bb_keys::damaged, false);
	//	BBC->SetValueAsBool(bb_keys::montage, false);
	//	
	//	GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
	//}

	if (!Manager) GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
	else if (Manager->GetClass()->ImplementsInterface(UManagerInterface::StaticClass()))
	{
		bool bSuccess = Cast<IManagerInterface>(Manager)->UpdatedAttackInManager(Agent);
		if(bSuccess) GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
	}
}

void AAIControllerBase::Setup_Perception_System()
{
	//�����ý����� �������ش�//
	Sight_Config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight_Config"));
	//AI��Ʈ�ѷ��� ���� ������Ʈ�� ��������
	Sight_Config->SightRadius = 1000.f;
	Sight_Config->LoseSightRadius = Sight_Config->SightRadius + 25.f;
	Sight_Config->PeripheralVisionAngleDegrees = 60.f;
	Sight_Config->SetMaxAge(5.f);
	Sight_Config->AutoSuccessRangeFromLastSeenLocation = 0.f;
	Sight_Config->DetectionByAffiliation.bDetectEnemies = true;
	Sight_Config->DetectionByAffiliation.bDetectFriendlies = true;
	Sight_Config->DetectionByAffiliation.bDetectNeutrals = true;
	///////////////////////////

	//����������Ʈ�� ����������Ʈ�� �Ѱ��ش�//
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));
	GetPerceptionComponent()->SetDominantSense(*Sight_Config->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnPerception);
	GetPerceptionComponent()->ConfigureSense(*Sight_Config);

	Hearing_Config = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing_Config"));
	Hearing_Config->HearingRange = 1000.0f;
	Hearing_Config->DetectionByAffiliation.bDetectEnemies = true;
	Hearing_Config->DetectionByAffiliation.bDetectFriendlies = true;
	Hearing_Config->DetectionByAffiliation.bDetectNeutrals = true;

	//GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnPerceptionUpdated);
	GetPerceptionComponent()->ConfigureSense(*Hearing_Config);
}
