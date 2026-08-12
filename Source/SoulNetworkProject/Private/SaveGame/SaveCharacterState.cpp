// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/SaveCharacterState.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/ObjectManager.h"

#include "Kismet/GameplayStatics.h"
#include "SoulNetworkDebug.h"


bool USaveCharacterState::SetCurrenWorldName(FString WorldName)
{
	if(WorldName == "") return false;

	for (auto& ActivateMap : ActivateMaps)
	{
		if (ActivateMap.WorldName == WorldName)
		{
			CurrentWorldName = WorldName;
			UGameplayStatics::SaveGameToSlot(this, UserName, 0);

			return true;
		}
	}

	return false;
}

bool USaveCharacterState::SetupData(FString InName, TArray<FDataContent> InContents, uint8 MaxPt, uint8 MaxHealthPt, uint8 MaxManaPt)
{
	if(InContents.Num() == 0 || MaxPt != MaxHealthPt + MaxManaPt) return false;


	UserName = InName;
	UserLevel = 1;
	UserSoul = 0;

	MaxPotionCount = MaxPt;

	MaxHealthPotion = MaxHealthPt;
	HealthData.bEquip = true;
	HealthData.EquipIndex = 0;
	HealthData.DataCount = MaxHealthPotion;
	//CurrentHealthPotion = MaxHealthPotion;

	MaxManaPotion = MaxManaPt;
	ManaData.bEquip = true;
	ManaData.EquipIndex = 1;
	ManaData.DataCount = MaxManaPotion;

	//CurrentManaPotion = MaxManaPotion;

	Contents = InContents;

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}

bool USaveCharacterState::SetMapData(FString WorldName, AActor* ObjectManager)
{
	AObjectManager* OM = Cast<AObjectManager>(ObjectManager);
	if(!OM) return false;

	for (auto& MapData : ActivateMaps)
	{
		if (MapData.WorldName == WorldName)
		{
			CurrentWorldName = WorldName;

			MapData.WorldName = WorldName;
			//MapData.bSOActives = bSOActives;
			//MapData.bAMActives = bAMAActives;
			//MapData.bBMActives = bBMActives;
			//MapData.bShowedSinematics = bShowedSinematics;

			UGameplayStatics::SaveGameToSlot(this, UserName, 0);
			return true;
		}
	}

	FMapData MapData;
	CurrentWorldName = WorldName;
	MapData.WorldName = WorldName;
	//MapData.bSOActives = bSOActives;
	//MapData.bAMActives = bAMAActives;
	//MapData.bBMActives = bBMActives;
	//MapData.bShowedSinematics = bShowedSinematics;

	ActivateMaps.Add(MapData);

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return false;
}

bool USaveCharacterState::SetRemainSoul(int64 InSoul, FVector NewLocation)
{
	if(CurrentWorldName == TEXT("")) return false;
	for (auto& ActiveMap : ActivateMaps)
	{
		if (ActiveMap.WorldName == CurrentWorldName)
		{
			ActiveMap.WorldRemainSoul.Soul = InSoul;
			ActiveMap.WorldRemainSoul.SpawnLocation = NewLocation;
		}
		else
		{
			ActiveMap.WorldRemainSoul.Soul = 0;
			ActiveMap.WorldRemainSoul.SpawnLocation = FVector::ZeroVector;
		}
	}

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}

bool USaveCharacterState::SetInvenIndex(uint8 InSpellIndex, uint8 InExpendableIndex, uint8 InL_WeaponIndex, uint8 InR_WeaponIndex)
{
	InSpellIndex >= 4 ? SpellIndex = 0 : SpellIndex = InSpellIndex;
	InExpendableIndex >= 8 ? ExpendableIndex = 0 : ExpendableIndex = InExpendableIndex;
	InL_WeaponIndex >= 3 ? L_WeaponIndex = 0 : L_WeaponIndex = InL_WeaponIndex;
	InR_WeaponIndex >= 3 ? R_WeaponIndex = 0 : R_WeaponIndex = InR_WeaponIndex;
	
	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}

bool USaveCharacterState::SetCondition(float InHealth, float InMana)
{
	if(InHealth <= 0 || InMana <= 0) return false;

	CurrentHealth = InHealth;
	CurrentMana = InMana;

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);


	return false;
}

bool USaveCharacterState::SetStatus(int8 Level, int64 Soul)
{
	//if(!InPlayer) return false;

	UserLevel = Level;
	UserSoul = Soul;

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}

bool USaveCharacterState::SetNewMapData(FString WorldName, TArray<FName> CheckNames)
{
	for (auto& ActivateMap : ActivateMaps)
	{
		if(ActivateMap.WorldName == WorldName) return false;
	}

	FMapData NewMapData;
	NewMapData.WorldName = WorldName;

	TArray<FObjectData> NewCheckPoints;
	for (auto& CheckName : CheckNames)
	{
		FObjectData CheckPointData;
		CheckPointData.ObjectName = CheckName.ToString();
		CheckPointData.Activates.Add(false);
	}

	ActivateMaps.Add(NewMapData);
	
	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}

bool USaveCharacterState::SetWorldItems(TArray<APickupActor*> WorldItems)
{
	for (auto& ActiveMap : ActivateMaps)
	{
		if (ActiveMap.WorldName == CurrentWorldName)
		{
			ActiveMap.WorldItems.Empty();
			for (auto& WorldItem : WorldItems)
			{
				FWorldItem NewData;
				NewData.Contents = WorldItem->GetDataContents();
				NewData.WorldTransform = WorldItem->GetActorTransform();

				ActiveMap.WorldItems.Add({WorldItem->GetDataContents(), WorldItem->GetActorTransform()});
			}
			break;
		}
	}

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}

bool USaveCharacterState::SetCheckPoints(TArray<FObjectData> CheckPointsData)
{
	for (auto& ActiveMap : ActivateMaps)
	{
		if (ActiveMap.WorldName == CurrentWorldName)
		{
			ActiveMap.CheckPoints = CheckPointsData;

			break;
		}
	}

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}

bool USaveCharacterState::SetBossManagers(TArray<FObjectData> BossManagersData)
{
	for (auto& ActiveMap : ActivateMaps)
	{
		if (ActiveMap.WorldName == CurrentWorldName)
		{
			ActiveMap.BossMaangers = BossManagersData;

			break;
		}
	}

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;

}
bool USaveCharacterState::SetAIManagers(TArray<FObjectData> AIManagersData)
{
	for (auto& ActiveMap : ActivateMaps)
	{
		if (ActiveMap.WorldName == CurrentWorldName)
		{
			ActiveMap.AIManagers = AIManagersData;
			SoulDebug::ScreenLog(UserName, FColor::Blue, 10.f);

			break;
		}
	}

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}
bool USaveCharacterState::SetSmartObjects(TArray<FObjectData> SmartObjectsData)
{
	for (auto& ActiveMap : ActivateMaps)
	{
		if (ActiveMap.WorldName == CurrentWorldName)
		{
			ActiveMap.SmartObjects = SmartObjectsData;

			break;
		}
	}

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;

}

bool USaveCharacterState::SetPotionData(FDataContent InHealthData, FDataContent InManaData)
{
	HealthData = InHealthData;
	ManaData = InManaData;

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}

bool USaveCharacterState::SetLastSpawnData(bool bRespawn, FTransform InLastTransform, FTransform InLastCPTransform)
{
	SpawnData.bRespawn = bRespawn;
	SpawnData.LastTransform = InLastTransform;
	//SpawnData.Name = InLastCPTransform;

	UGameplayStatics::SaveGameToSlot(this, UserName, 0);

	return true;
}

FMapData USaveCharacterState::GetMapData(FString RowName)
{
	FMapData OutputData{};
	for (auto& ActiveMap : ActivateMaps)
	{
		if(ActiveMap.WorldName == RowName) OutputData = ActiveMap;
	}

	return OutputData;
}

FString USaveCharacterState::GetFrontStackWorldName()
{
	FString outString;

	if(ActivateMaps.Num() > 0)	outString = ActivateMaps[0].WorldName;

	return outString;
}

FString USaveCharacterState::GetLastStackWorldName()
{
	FString outString;

	if (ActivateMaps.Num() > 0)	outString = ActivateMaps.Last().WorldName;

	return outString;
}
