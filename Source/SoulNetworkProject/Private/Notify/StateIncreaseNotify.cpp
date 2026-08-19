// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/StateIncreaseNotify.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/EffectComponent.h"


FString UStateIncreaseNotify::GetNotifyName_Implementation() const
{
	return L"State_Increase";
}

void UStateIncreaseNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ACharacterBase* Chr = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Chr->GetStateComponent())
		{
			switch (IncreaseType)
			{
				case EIncreaseType::Expendable:
				{
					if (Chr->GetIncreaseData().IsSet() && Chr->GetIncreaseData().ItemCount > 0)
					{
						if (Chr->GetRemoteRole() == ROLE_AutonomousProxy || Chr->GetLocalRole() == ROLE_Authority)
						{
							if(Chr->GetIncreaseData().ItemData.ItemType != EItemType::Expendable) return;
							Chr->GetStateComponent()->UpdatedIncreaseData();
						}

						if (APlayerCharacter* Player = Cast<APlayerCharacter>(Chr))
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
				break;
				case EIncreaseType::Value:
				{
					//Player->GetStateComponent()->Server_AddStamina(-10.f);
					Chr->GetStateComponent()->AddStamina(-IncreaseValue);
				}
				break;
			}
		}
	}
}