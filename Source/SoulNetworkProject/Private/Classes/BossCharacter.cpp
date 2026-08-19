// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/BossCharacter.h"
#include "Classes/AIControllerBase.h"
#include "Classes/PlayerCharacter.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "Classes/BossManager.h"
#include "Classes/CombatComponent.h"
#include "Classes/EffectComponent.h"
#include "Classes/CharacterStateComponent.h"


#include "SoulNetworkProject/CharacterState.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(BossManager)	GetEffectComponent()->AllMeshFadeout(BossManager->GetShowedCinematic() ? 1.0f : 0.0f);
}

void ABossCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void ABossCharacter::StartBossStage(ACharacterBase* EnterActor)
{
	if(!EnterActor || !ControllerRef) return;

	//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green, EnumToString<EAIState>(static_cast<EAIState>(ControllerRef->GetBlackboard()->GetValueAsEnum(bb_keys::ai_status))));

	
	ControllerRef->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, BossManager->GetShowedCinematic() ? (uint8)EAIState::Attack : (uint8)EAIState::Idle);
	BossManager->SetShowedCinemaitc(true);
	if (!ControllerRef->GetBlackboard()->GetValueAsObject(bb_keys::target_actor))
	{
		ControllerRef->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, EnterActor);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, EnumToString<EAIState>(static_cast<EAIState>(ControllerRef->GetBlackboard()->GetValueAsEnum(bb_keys::ai_status))));


}

void ABossCharacter::SetManager(ABossManager* OwnerManager)
{
	BossManager = OwnerManager;
}

bool ABossCharacter::HasActorInArea(AActor* CheckActor)
{
	for (auto& Chr : BossManager->GetInAreaActor())
	{

		if(Chr == CheckActor) return true;
	}
	return false;
}

void ABossCharacter::SetupCharacter(bool bActivate)
{

	//if(!GetEffectComponent()) return;

	SetDead(bActivate);
	SetActorHiddenInGame(bActivate);
	//BossManager->UpdatedBossManager();

	//if (bActivate)
	//{
	//	//GetEffectComponent()->AllMeshFadeout(1.0f);
	//	//GetEffectComponent()->AllMeshHidden(false);
	//	SetDead(false);
	//	SetActorHiddenInGame(false);
	//	BossManager->UpdatedBossManager();
	//}
	//else
	//{
	//	//GetEffectComponent()->AllMeshHidden(true);
	//	SetDead(true);
	//	SetActorHiddenInGame(true);
	//	BossManager->UpdatedBossManager();
	//
	//}
}

int8 ABossCharacter::GetBossPhaseIndex() const
{
	int8 Index = 0;
	
	if (GetStateComponent())
	{
		if (BossPhaseState.PhaseType == EPhaseType::Enchant)
		{
			Index = GetStateComponent()->GetEnchant().IsSet() ? 1 : 0;
		}
		else if (BossPhaseState.PhaseType == EPhaseType::Enchant)
		{
			Index = BossPhaseState.PhaseIndex;
		}
	}

	return Index;
}


void ABossCharacter::EnemyDeathEvent()
{
	if (BossManager && GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
	{
		BossManager->UpdatedBossManager();
		FadeInOut(true);
		for (auto& Chr : BossManager->GetInAreaActor())
		{
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(Chr))
			{
				for (auto& Content : DropItems)
				{
					FInventoryData Data = SearchInvenData(this, Content, true);
					if (Data.IsSet())
					{
						if (Player->GetRemoteRole() == ROLE_AutonomousProxy)
						{
							Player->GetInventoryComponent()->Client_AddItem(Data, true);
						}
						else if (Player->GetLocalRole() == ROLE_Authority)
						{
							Player->GetInventoryComponent()->AddItem(Data, true);
						}
					}
				}
				Cast<IGameStateInterface>(GetWorld()->GetGameState())->SetBossWidget(false, Player);
				Player->Client_BindBossActor(nullptr);
			}
		}
		Cast<IGameStateInterface>(GetWorld()->GetGameState())->CallEvnetUI(BossManager->GetInAreaActor(), EEventUIType::FELLED);
	}
}

void ABossCharacter::Server_SkillMontage_Implementation(uint8 SkillIndex)
{
	Multicast_SkillMontage(SkillIndex);
}
bool ABossCharacter::Server_SkillMontage_Validate(uint8 SkillIndex)
{
	return true;
}

void ABossCharacter::Multicast_SkillMontage_Implementation(uint8 SkillIndex)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;


	AnimValues.bAOEAttack = true;

	AnimInstance->Montage_Play(SkillArr[SkillIndex]);
}

bool ABossCharacter::Multicast_SkillMontage_Validate(uint8 SkillIndex)
{
	return true;
}
