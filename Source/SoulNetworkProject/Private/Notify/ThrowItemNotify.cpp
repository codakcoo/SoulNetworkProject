// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/ThrowItemNotify.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/EnemyCharacter.h"
#include "Classes/CombatComponent.h"

FString UThrowItemNotify::GetNotifyName_Implementation() const
{
	return L"Throw_Item";
}

void UThrowItemNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		//if (Chr->GetIncreaseData().IsSet() && Chr->GetIncreaseData().ItemCount > 0)
		{
			if (Chr->GetRemoteRole() == ROLE_AutonomousProxy || Chr->GetLocalRole() == ROLE_Authority)
			{
				if (ThrowType == EThrowType::Expendable)
				{
					Chr->Server_SpawnExpendableThrowActor();
				}
				else if (ThrowType == EThrowType::Arrow)
				{
					Chr->GetUseHeavyArrow() ? Chr->Server_SpawnHeavyArrowActor() : Chr->Server_SpawnLowArrowActor();

				}
			}

			if (APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
			{
				if (Player->GetRemoteRole() == ROLE_AutonomousProxy)
				{
					Player->GetInventoryComponent()->Client_RefreshExpendableItem(Player, Player->GetIncreaseData());
				}
				else if (Player->GetLocalRole() == ROLE_Authority)
				{
					Player->GetInventoryComponent()->UpdatedItemCount(Player->GetIncreaseData());
				}
			}
		}
	}
	//else if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(MeshComp->GetOwner()))
	//{
	//	if (Enemy->GetIncreaseData().IsSet())
	//	{
	//		if (!Enemy->GetCombatComponent()) return;
	//
	//		//Enemy->GetCombatComponent()->Server_SpawnThrowItem();
	//		Enemy->Server_SpawnThrowActor();
	//	}
	//}
}
