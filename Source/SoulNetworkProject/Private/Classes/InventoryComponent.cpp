// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/InventoryComponent.h"
#include "Classes/PlayerCharacter.h"
#include "Interface/InventoryInterface.h"
#include "Interface/CharacterInterface.h"
#include "SoulNetworkProject/PlayerHUD.h"

#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

#define FIRSTSLOT 0
#define SECONDSLOT 1
#define THIRDSLOT 2
#define POTIONTOOL	TEXT("PotionUpgrade")


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	Capacity = 20;
	SetIsReplicated(true);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerPawn = Cast<APlayerCharacter>(GetOwner());

	if (OwnerPawn)
	{
		//if (!OwnerPawn->GetPlayerState()) return;
		//if (OwnerPawn->GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		//{
		//	Cast<ICharacterInterface>(OwnerPawn->GetPlayerState())->SetupState(OwnerPawn);
		//}
//#if WITH_EDITOR
//		GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Cyan, __FUNCTION__);
//#endif
		PermuteDataContentsToItemData();
		AllRefreshSlots();
	}
}

bool UInventoryComponent::AddItem(FInventoryData& Item, bool bShowList)
{

	Item.StackIndex = Items.Num() - 1;

	if (Item.ItemData.ItemType == EItemType::Expendable || Item.ItemData.ItemType == EItemType::Arrow || Item.ItemData.ItemType == EItemType::Enhancement)
	{
		if (!GetCountItem(Item.ItemData.ItemName))
		{

			Items.Add(Item);

		}
		else
		{
			FInventoryData OuputData = *GetCountItem(Item.ItemData.ItemName);
			if (GetCountItem(Item.ItemData.ItemName)->ItemCount + Item.ItemCount >= 99)
			{
				int32 OverCount = GetCountItem(Item.ItemData.ItemName)->ItemCount + Item.ItemCount - 99;

				GetCountItem(Item.ItemData.ItemName)->ItemCount = 99;
				Item.ItemCount = OverCount;

				if(OuputData.bEquip) GetItem(Item.ItemData.ItemType, OuputData.EquipIndex)->ItemCount = 99;

				Items.Add(Item);
			}
			else
			{
				GetCountItem(Item.ItemData.ItemName)->ItemCount += Item.ItemCount;

				if (OuputData.bEquip) GetItem(Item.ItemData.ItemType, OuputData.EquipIndex)->ItemCount += Item.ItemCount;
			}
		}
	}
	else
	{
		Items.Add(Item);
	}
	if(bShowList) OnViewListUpdated.Broadcast(Item);

	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();

	return true;
}

bool UInventoryComponent::RemoveItem(FInventoryData& Item)
{
	//�������� ��� �ش� ��ġâ�� �ִ� �������� ������//
	if (Item.ItemData.ItemType != EItemType::Expendable && Item.ItemData.ItemType != EItemType::Arrow)
	{
		if (Item.bEquip)
		{
			RemoveEquipItem(Item, Item.EquipIndex, Item.HandType);
		}
		Items.RemoveSingle(Item);
	}
	else
	{

		TArray<FInventoryData> BackupDatas;
		for (auto& Data : Items)
		{
			if (Data.ItemData.ItemName == Item.ItemData.ItemName)
			{
				BackupDatas.Add(Data);

				if (Data.StackIndex == Item.StackIndex && Data.ItemCount == Item.ItemCount)
				{
					if (Item.bEquip)
					{
						RemoveEquipItem(Item, Item.EquipIndex, Item.HandType);
					}
				}
			}
		}

		if (BackupDatas.Num() == 1)
		{
			if(BackupDatas[0].ItemCount - Item.ItemCount == 0)
			{
				Items.RemoveSingle(Item);
				BackupDatas.Empty();
			}
			else
			{
				BackupDatas[0].ItemCount -= Item.ItemCount;
			}
		}
		else
		{
			if (BackupDatas.Last().ItemCount - Item.ItemCount == 0) Items.RemoveSingle(BackupDatas.Last());
			else if (BackupDatas[BackupDatas.Num() - 1].ItemCount - Item.ItemCount > 0)
			{
				BackupDatas[BackupDatas.Num() - 1].ItemCount -= Item.ItemCount;
			}
			else
			{
				int8 overCount = BackupDatas.Last().ItemCount - Item.ItemCount;
				Items.RemoveSingle(BackupDatas.Last());
				BackupDatas.RemoveSingle(BackupDatas.Last());
				BackupDatas.Last().ItemCount += overCount;
			}
		}
		for (auto& BackupData : BackupDatas)
		{
			for (auto& Data : Items)
			{
				if (Data.ItemData.ItemName == Item.ItemData.ItemName)
				{
					if (Data.StackIndex == BackupData.StackIndex)
					{
						Data.ItemCount = BackupData.ItemCount;
						break;
					}
				}
			}
		}
	}


	OnInventoryUpdated.Broadcast();
	return true;
}

void UInventoryComponent::Client_AddItem_Implementation(FInventoryData Item, bool bShowList)
{

	//Item.StackIndex = Items.Num() - 1;
	//
	//if (Item.ItemData.ItemType == EItemType::Expendable)
	//{
	//	if (!GetSameExpendableData(Item.ItemData.ItemName))
	//	{
	//
	//		Items.Add(Item);
	//	}
	//	else
	//	{
	//		FInventoryData OuputData = *GetSameExpendableData(Item.ItemData.ItemName);
	//		if (GetSameExpendableData(Item.ItemData.ItemName)->ItemCount + Item.ItemCount > 99)
	//		{
	//			int32 OverCount = GetSameExpendableData(Item.ItemData.ItemName)->ItemCount + Item.ItemCount - 99;
	//
	//			GetSameExpendableData(Item.ItemData.ItemName)->ItemCount = 99;
	//			Item.ItemCount = OverCount;
	//
	//			if (OuputData.bEquip) GetItem(EItemType::Expendable, OuputData.EquipIndex)->ItemCount = 99;
	//
	//			Items.Add(Item);
	//		}
	//		else
	//		{
	//			GetSameExpendableData(Item.ItemData.ItemName)->ItemCount += Item.ItemCount;
	//
	//			if (OuputData.bEquip) GetItem(EItemType::Expendable, OuputData.EquipIndex)->ItemCount += Item.ItemCount;
	//		}
	//
	//	}
	//}
	//else
	//{
	//	Items.Add(Item);
	//}
	//if (bShowList) OnViewListUpdated.Broadcast(Item);
	//
	//OnInventoryUpdated.Broadcast();
	//OnEquipmentUpdated.Broadcast();

	//if (Items.Num() >= Capacity) return false;


	Item.StackIndex = Items.Num() - 1;

	if (Item.ItemData.ItemType == EItemType::Expendable || 
		Item.ItemData.ItemType == EItemType::Arrow || 
		Item.ItemData.ItemType == EItemType::Enhancement)
	{
		if (!GetCountItem(Item.ItemData.ItemName))
		{

			Items.Add(Item);

		}
		else
		{
			FInventoryData OuputData = *GetCountItem(Item.ItemData.ItemName);
			if (GetCountItem(Item.ItemData.ItemName)->ItemCount + Item.ItemCount >= 99)
			{
				int32 OverCount = GetCountItem(Item.ItemData.ItemName)->ItemCount + Item.ItemCount - 99;

				GetCountItem(Item.ItemData.ItemName)->ItemCount = 99;
				Item.ItemCount = OverCount;

				if (OuputData.bEquip) GetItem(Item.ItemData.ItemType, OuputData.EquipIndex)->ItemCount = 99;

				Items.Add(Item);
			}
			else
			{
				GetCountItem(Item.ItemData.ItemName)->ItemCount += Item.ItemCount;

				if (OuputData.bEquip) GetItem(Item.ItemData.ItemType, OuputData.EquipIndex)->ItemCount += Item.ItemCount;
			}
		}
	}
	else
	{
		Items.Add(Item);
	}
	if (bShowList) OnViewListUpdated.Broadcast(Item);

	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();
}

bool UInventoryComponent::Client_AddItem_Validate(FInventoryData Item, bool bShowList = false)
{
	return true;
}

void UInventoryComponent::UseItem(FInventoryData& Item)
{
	OwnerPawn->Server_UseExpendable(Item);
}

void UInventoryComponent::DropItem(FInventoryData& Item, uint8 DropCount)
{
	if (Item.ItemData.ItemType != EItemType::Expendable)
	{
		OwnerPawn->DropItem(Item, OwnerPawn->UpdatedLastLocation());
		RemoveItem(Item);
	}
	else
	{
		FInventoryData DropData;
		DropData = Item;

		if (Item.ItemCount == DropCount)
		{
			OwnerPawn->DropItem(Item, OwnerPawn->UpdatedLastLocation());
			RemoveItem(Item);
		}
		else if (DropCount > 0 || DropCount < Item.ItemCount)
		{
			DropData.ItemCount = DropCount;
			GetCountItem(Item.ItemData.ItemName)->ItemCount -= DropCount;
			OwnerPawn->DropItem(DropData, OwnerPawn->UpdatedLastLocation());

			OnInventoryUpdated.Broadcast();
		}
	}
}

void UInventoryComponent::DestroyItem(FInventoryData& Item, uint8 DestroyCount)
{
	if (Item.ItemData.ItemType != EItemType::Expendable)
	{
		RemoveItem(Item);
	}
	else
	{
		FInventoryData DropData;
		DropData = Item;

		if (Item.ItemCount == DestroyCount)
		{
			RemoveItem(Item);
		}
		else if (DestroyCount > 0 || DestroyCount < Item.ItemCount)
		{
			GetCountItem(Item.ItemData.ItemName)->ItemCount -= DestroyCount;

			OnInventoryUpdated.Broadcast();
		}
	}
}


void UInventoryComponent::DropAtOnceItem(TArray<FInventoryData> DropItems)
{
	if(DropItems.Num() < 1 || DropItems.Num() > 5) return;

	for (auto& Item : DropItems)
	{
		OwnerPawn->DropItem(Item, OwnerPawn->UpdatedLastLocation());
		RemoveItem(Item);
	}

	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::DestroyAtOnceItem(TArray<FInventoryData> DestroyItems)
{
	if (DestroyItems.Num() < 1 || DestroyItems.Num() > 5) return;

	for (auto& Item : DestroyItems)
	{
		RemoveItem(Item);
	}

	OnInventoryUpdated.Broadcast();
}


//�κ��丮���� �� �ε����� �������� �߰���//
bool UInventoryComponent::AddEquipItem(FInventoryData& Item, int32 Index, EEquipHand HandType)
{

	SetEquipAndIndex(Item, true, Index);
	Item.bEquip = true;
	Item.EquipIndex = Index;

	switch (Item.ItemData.ItemType)
	{
		case EItemType::All:
		{
			SetEquipAndIndex(Item, false, 0);
			Item.bEquip = false;
			Item.EquipIndex = 0;
		}
		break;

		case EItemType::Weapon:
		{
			if (HandType == EEquipHand::None || Index < 0 || Index > 2)
			{
				SetEquipAndIndex(Item, false, 0);
				Item.bEquip = false;
				Item.EquipIndex = 0;

				return false;
			}

			Item.EquipIndex = Index;
			Item.HandType = HandType;
			if (HandType == EEquipHand::Left)
			{
				L_WeaponEquip.Add(Index, Item);
				OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Left, 
					GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) ? 
					*GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) : FInventoryData{},
					false);
			}
			else if (HandType == EEquipHand::Right)
			{
				R_WeaponEquip.Add(Index, Item);
				OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Right, 
					GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) ?
					*GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) : FInventoryData{},
					false);
			}
		}
		break;

		case EItemType::Helmet:
		{
			HelmetEquip = Item;
			OnEquipSlotUpdated.Broadcast(EItemType::Helmet, EEquipHand::None, 
				GetItem(EItemType::Helmet) ?
				*GetItem(EItemType::Helmet) : FInventoryData{},
				false);
		}
		break;

		case EItemType::Armor:
		{
			ArmorEquip = Item; 
			OnEquipSlotUpdated.Broadcast(EItemType::Armor, EEquipHand::None, 
				GetItem(EItemType::Armor) ?
				*GetItem(EItemType::Armor) : FInventoryData{},
				false);
		}
		break;

		case EItemType::Pant:
		{
			PantEquip = Item;

			OnEquipSlotUpdated.Broadcast(EItemType::Pant, EEquipHand::None, 
				GetItem(EItemType::Pant) ?
				*GetItem(EItemType::Pant) : FInventoryData{},
				false);
		}
		break;

		case EItemType::Boots:
		{
			BootsEquip = Item;

			OnEquipSlotUpdated.Broadcast(EItemType::Boots, EEquipHand::None, 
				GetItem(EItemType::Boots) ? 
				*GetItem(EItemType::Boots) : FInventoryData{},
				false);
		}
		break;

		case EItemType::Expendable:
		{
			if (Index < 0 || Index > 7)
			{
				SetEquipAndIndex(Item, false, 0);
				Item.bEquip = false;
				Item.EquipIndex = 0;

				return false;
			}
			//SetEquipType(Item, Index, HandType);
			//if (ExpendableEquip.Num() == 0)
			//{
			//	ExpendableIndex = Index;
			//}
			Item.EquipIndex = Index;
			ExpendableEquip.Add(Index, Item);


			OnEquipSlotUpdated.Broadcast(EItemType::Expendable, EEquipHand::None, 
				GetItem(EItemType::Expendable, ExpendableIndex) ?
				*GetItem(EItemType::Expendable, ExpendableIndex) : FInventoryData{},
				false);

		}
		break;

		case EItemType::Arrow:
		{
			//FirstArrowEquip = Item;
			Item.EquipIndex = Index;
			Item.bEquip = true;
		
			ArrowEquip.Add(Index, Item);
			//OwnerPawn->AttachEquipItem(*GetItem(EItemType::Arrow));

			OnEquipSlotUpdated.Broadcast(EItemType::Arrow, EEquipHand::None, 
				GetItem(EItemType::Arrow, Index) ? 
				*GetItem(EItemType::Arrow, Index) : FInventoryData{},
				false);

		}
		break;

		case EItemType::Spell:
		{	
			if (Index < 0 || Index > 3)
			{
				SetEquipAndIndex(Item, false, 0);
				Item.bEquip = false;
				Item.EquipIndex = 0;

				return false;
			}
			//SetEquipType(Item, Index, HandType);
			Item.EquipIndex = Index;
			SpellEquip.Add(Index, Item);

			//if (SpellEquip.Num() == 1)
			//{
			//	SpellIndex = Index;
			//}

			OnEquipSlotUpdated.Broadcast(EItemType::Spell, EEquipHand::None, 
				GetItem(EItemType::Spell, SpellIndex) ? 
				*GetItem(EItemType::Spell, SpellIndex) : FInventoryData{},
				false);
		}
		break;

		default:
		{
			SetEquipAndIndex(Item, false, 0);
			Item.bEquip = false;
			Item.EquipIndex = 0;
		}
		break;
	}

	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();

	return true;
}
//////////////////////////////////

//Data�� ����//
//�ش� �������� ��ġ�� �ű�ų� ��ü �Ͽ������ ��ü�� (Item ���뿩�ε� ��ü��)//
bool UInventoryComponent::RemoveEquipItem(FInventoryData& Item, int32 Index, EEquipHand HandType)
{
	SetEquipAndIndex(Item, false, 0);
	Item.bEquip = false;
	Item.EquipIndex = 0;

	switch (Item.ItemData.ItemType)
	{
		case EItemType::All:
		{
			SetEquipAndIndex(Item, true, Index);
			Item.bEquip = true;
			Item.EquipIndex = Index;
		}
		break;
		case EItemType::Weapon:
		{
			if (HandType == EEquipHand::None || Index < 0 || Index > 2)
			{
				SetEquipAndIndex(Item, true, Index);
				Item.bEquip = true;
				Item.EquipIndex = Index;

				return false;
			}
			Item.EquipIndex = 0;
			Item.HandType = EEquipHand::None;

			if (HandType == EEquipHand::Left)
			{
				L_WeaponEquip.Remove(Index);
				OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Left, 
					GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) ? 
					*GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) : FInventoryData{},
					false);

			}
			else if (HandType == EEquipHand::Right)
			{

				R_WeaponEquip.Remove(Index);
				OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Right, 
					GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) ? 
					*GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) : FInventoryData{},
					false);

			}
		}
		break;
		case EItemType::Helmet:
		{
			HelmetEquip.ResetValue();
			OnEquipSlotUpdated.Broadcast(EItemType::Helmet, EEquipHand::None, 
				GetItem(EItemType::Helmet) ?
				*GetItem(EItemType::Helmet) : FInventoryData{},
				false);

		}
		break;
		case EItemType::Armor:
		{
			ArmorEquip.ResetValue();
			OnEquipSlotUpdated.Broadcast(EItemType::Armor, EEquipHand::None, 
				GetItem(EItemType::Armor) ?
				*GetItem(EItemType::Armor) : FInventoryData{},
				false);
		}
		break;
		case EItemType::Pant:
		{
			PantEquip.ResetValue();
			OnEquipSlotUpdated.Broadcast(EItemType::Pant, EEquipHand::None, 
			GetItem(EItemType::Pant) ?
			*GetItem(EItemType::Pant) : FInventoryData{},
			false);
		}
		break;
		case EItemType::Boots:
		{
			BootsEquip.ResetValue();
			OnEquipSlotUpdated.Broadcast(EItemType::Boots, EEquipHand::None, 
				GetItem(EItemType::Boots) ?
				*GetItem(EItemType::Boots) : FInventoryData{},
				false);
		}
		break;
		case EItemType::Expendable:
		{

			if (Index < 0 || Index > 7)
			{
				SetEquipAndIndex(Item, true, Index);
				Item.bEquip = true;
				Item.EquipIndex = Index;

				return false;
			}

			if (ExpendableEquip.Num() == 0) ExpendableIndex = 0;
			else
			{
				for (int8 i = ExpendableIndex == GetMaxExpendableIndex() ? 0 : ExpendableIndex; i < 8; i++)
				{
					if (GetItem(EItemType::Expendable, i))
					{
						ExpendableIndex = i;
						break;
					}
				}
			}

			ExpendableEquip.Remove(Index);
			OnEquipSlotUpdated.Broadcast(EItemType::Expendable, EEquipHand::None, 
				GetItem(EItemType::Expendable, ExpendableIndex) ?
				*GetItem(EItemType::Expendable, ExpendableIndex) : FInventoryData{},
				false);
		}
		break;

		case EItemType::Arrow:
		{
			//OwnerPawn->RemoveEquipItem(EItemType::Arrow);
			ArrowEquip.Remove(Index);

			OnEquipSlotUpdated.Broadcast(EItemType::Arrow, EEquipHand::None, 
				GetItem(EItemType::Arrow, Index) ?
				*GetItem(EItemType::Arrow, Index) : FInventoryData{},
				false);
		}
		break;

		case EItemType::Spell:
		{
			//if (SpellEquip.Num() == 0) SpellIndex = 0;
			//else
			//{
			//	for (int8 i = SpellIndex == GetMaxSpellIndex() ? 0 : SpellIndex; i < 8; i++)
			//	{
			//		if (GetItem(EItemType::Spell, i))
			//		{
			//			SpellIndex = i;
			//			break;
			//		}
			//	}
			//}

			if (Index < 0 || Index > 4)
			{
				SetEquipAndIndex(Item, true, Index);
				Item.bEquip = true;
				Item.EquipIndex = Index;

				return false;
			}

			SpellEquip.Remove(Index);
			OnEquipSlotUpdated.Broadcast(EItemType::Spell, EEquipHand::None, 
				GetItem(EItemType::Spell, SpellIndex) ? 
				*GetItem(EItemType::Spell, SpellIndex) : FInventoryData{},
				false);

		}
		break;
		default:
		{
			SetEquipAndIndex(Item, true, Index);
			Item.bEquip = true;
			Item.EquipIndex = Index;
		}
		break;
	}

	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();
	return true;
}
//////////////////////////////////////////////////

//���� ������ ��ȯ//
FInventoryData* UInventoryComponent::GetItem(EItemType ItemType, uint8 Index, EEquipHand HandType)
{
	switch (ItemType)
	{
		case EItemType::Weapon:
		{
			if(HandType == EEquipHand::Right) return R_WeaponEquip.Find(Index) ? R_WeaponEquip.Find(Index) : nullptr;
			if(HandType == EEquipHand::Left) return L_WeaponEquip.Find(Index) ? L_WeaponEquip.Find(Index) : nullptr;
		}
		break;

		case EItemType::Helmet:
		return (&HelmetEquip);
		case EItemType::Armor:
		return (&ArmorEquip);
		case EItemType::Pant:
		return (&PantEquip);
		case EItemType::Boots:
		return (&BootsEquip);

		case EItemType::Expendable:
		return ExpendableEquip.Find(Index);

		case EItemType::Arrow:
		return ArrowEquip.Find(Index);

		case EItemType::Spell:
		return SpellEquip.Find(Index);

		default:
		break;
	}

	return nullptr;
}

FInventoryData* UInventoryComponent::GetIndexDataFromArray(FInventoryData SearchData)
{
	for (auto& Data : Items)
	{
		if (Data == SearchData)
		{
			return &Data;
		}
	}

	return nullptr;
}

FInventoryData* UInventoryComponent::GetCountItem(FName DataName)
{
	for (auto& Item : Items)
	{
		if (Item.ItemData.ItemType == EItemType::Expendable || 
			Item.ItemData.ItemType == EItemType::Arrow || 
			Item.ItemData.ItemType == EItemType::Enhancement)
		{
			if (Item.ItemData.ItemType == EItemType::Expendable && Item.ExpendableData.ExpendableType != EExpendableType::Potion) continue;
			if (Item.ItemData.ItemName == DataName)
			{
				if(Item.ItemCount < 99) return &Item;
			}
		}
	}
	return nullptr;
}

FInventoryData* UInventoryComponent::GetItemForSameName(FName DataName)
{
	for (auto& Item : Items)
	{
		if(Item.ItemData.ItemName == DataName) return &Item;
	}
	return nullptr;
}

void UInventoryComponent::SetSpellIndex(uint8 Index)
{
	SpellIndex >= 4 ? SpellIndex = 0 : SpellIndex = Index;

	OnEquipSlotUpdated.Broadcast(EItemType::Spell, EEquipHand::None,
		GetItem(EItemType::Spell, SpellIndex) ?
		*GetItem(EItemType::Spell, SpellIndex) : FInventoryData{},
		false);
}

void UInventoryComponent::SetExpendableIndex(uint8 Index)
{
	ExpendableIndex >= 8 ? ExpendableIndex = 0 : ExpendableIndex = Index;

	OnEquipSlotUpdated.Broadcast(EItemType::Expendable, EEquipHand::None,
		GetItem(EItemType::Expendable, ExpendableIndex) ?
		*GetItem(EItemType::Expendable, ExpendableIndex) : FInventoryData{},
		false);
}

void UInventoryComponent::SetL_AttachIndex(uint8 Index)
{
	L_AttachIndex >= 3 ? L_AttachIndex = 0 : L_AttachIndex = Index;

	OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Left,
		GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) ?
		*GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) : FInventoryData{},
		false);
}

void UInventoryComponent::SetR_AttachIndex(uint8 Index)
{
	R_AttachIndex >= 3 ? R_AttachIndex = 0 : R_AttachIndex = Index;

	OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Right,
		GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) ?
		*GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) : FInventoryData{},
		false);
}

void UInventoryComponent::Client_RefreshExpendableItem_Implementation(APlayerCharacter* CalledOwner, FInventoryData Data)
{
	if(CalledOwner != OwnerPawn) return;

	if (FInventoryData* RowData = GetIndexDataFromArray(Data))
	{
		//GetIndexDataFromArray(Data)->ItemCount--;
		//Data.ItemCount--;
		RowData->ItemCount--;
		if (RowData->ItemCount == 0)
		{
			if (RowData->ItemData.ItemName != "HealthPotion" && RowData->ItemData.ItemName != "ManaPotion")
			{
				RemoveItem(*RowData);
			}
		}
	}
	if (FInventoryData* RowData = GetItem(Data.ItemData.ItemType, Data.EquipIndex))
	{
		//GetItem(EItemType::Expendable, Data.EquipIndex)->ItemCount--;
		RowData->ItemCount--;

		if (RowData->ItemCount == 0)
		{
			if (RowData->ItemData.ItemName != "HealthPotion" && RowData->ItemData.ItemName != "ManaPotion")
			{
				RemoveItem(*RowData);
			}
		}
	}
	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();


	OnEquipSlotUpdated.Broadcast(Data.ItemData.ItemType, Data.HandType,
		GetItem(Data.ItemData.ItemType, Data.EquipIndex) ?
		*GetItem(Data.ItemData.ItemType, Data.EquipIndex, Data.HandType) : FInventoryData{},
		false);
}

bool UInventoryComponent::Client_RefreshExpendableItem_Validate(APlayerCharacter* CalledOwner, FInventoryData Data)
{
	return true;
}


FInventoryData* UInventoryComponent::GetPotionUpgradeItem()
{
	for (auto& Item : Items)
	{
		if (Item.ItemData.ItemType == EItemType::Enhancement)
		{
			if(Item.ItemData.ItemName == POTIONTOOL) return &Item;
		}
	}
	return nullptr;
}

void UInventoryComponent::SwitchLeftWeapon()
{
	if (!OwnerPawn) return;

	L_AttachIndex++;
	if (L_AttachIndex > 2) L_AttachIndex = 0;
	OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Left, 
		GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) ?
		*GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) : FInventoryData{},
		true);
}

void UInventoryComponent::SwitchRightWeapon()
{
	if (!OwnerPawn) return;

	R_AttachIndex++;
	if (R_AttachIndex > 2) R_AttachIndex = 0;
	OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Right, 
		GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) ?
		*GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) : FInventoryData{},
		true);
}

void UInventoryComponent::SwitchExpendable()
{
	if(!OwnerPawn) return;

	if (ExpendableEquip.Num() == 0)
	{
		ExpendableIndex = 0;
	}
	else if (ExpendableEquip.Num() == 1)
	{
		for (int8 i = 0; i < 8; i++)
		{
			if (GetItem(EItemType::Expendable, i))
			{
				ExpendableIndex = i;
				break;
			}
		}
	}
	else
	{
		for (int8 i = ExpendableIndex == GetMaxExpendableIndex() ? 0 : ExpendableIndex; i < 8; i++)
		{
			if (GetItem(EItemType::Expendable, i))
			{
				if (GetItem(EItemType::Expendable, ExpendableIndex) != GetItem(EItemType::Expendable, i))
				{
					ExpendableIndex = i;
					break;
				}
			}
		}
	}

	OnEquipSlotUpdated.Broadcast(EItemType::Expendable, EEquipHand::None, 
		GetItem(EItemType::Expendable, ExpendableIndex) ?
		*GetItem(EItemType::Expendable, ExpendableIndex) : FInventoryData{}, 
		true);
}

void UInventoryComponent::SwitchSpell()
{
	if (!OwnerPawn) return;

	if (SpellEquip.Num() == 0)
	{
		SpellIndex = 0;
	}
	else if (SpellEquip.Num() == 1)
	{
		for (int8 i = 0; i < 8; i++)
		{
			if (GetItem(EItemType::Spell, i))
			{
				SpellIndex = i;
				break;
			}
		}
	}
	else
	{
		for (int8 i = SpellIndex == GetMaxSpellIndex() ? 0 : SpellIndex; i < 8; i++)
		{
			if (GetItem(EItemType::Spell, i))
			{
				if (GetItem(EItemType::Spell, SpellIndex) != GetItem(EItemType::Spell, i))
				{
					SpellIndex = i;
					break;
				}
			}
		}
	}
	OnEquipSlotUpdated.Broadcast(EItemType::Spell, EEquipHand::None, 
		GetItem(EItemType::Spell, SpellIndex) ?
		*GetItem(EItemType::Spell, SpellIndex) : FInventoryData{}, 
		true);
}

void UInventoryComponent::SetData_Implementation(const TArray<FDataContent>& InDataContents)
{
	for (auto& Data : InDataContents)
	{
		DataContents.Add(Data);

		//GEngine->AddOnScreenDebugMessage(-1,3.0f, FColor::Blue, Data.DataName.ToString());
	}
}

FDataContent UInventoryComponent::ChangeItemDataToContent(FInventoryData ItemData)
{
	FDataContent OutContent;
	OutContent.DataName = ItemData.ItemData.ItemName;
	OutContent.DataCount = ItemData.ItemCount;

	OutContent.bEquip = ItemData.bEquip;
	OutContent.HandType = ItemData.HandType;
	OutContent.EquipIndex = ItemData.EquipIndex;

	return OutContent;
}

TArray<FDataContent> UInventoryComponent::PermuteItemDataToContents()
{
	TArray<FDataContent> InputContents;
	RemoveBasicItems({ TEXT("HealthPotion"), TEXT("ManaPotion") });

	for (auto& Item : Items)
	{
		FDataContent RowData;

		RowData.DataName = Item.ItemData.ItemName;
		RowData.DataCount = Item.ItemCount;
		RowData.bEquip = Item.bEquip;
		RowData.EquipIndex = Item.bEquip ? Item.EquipIndex : 0;
		RowData.HandType = Item.bEquip ? Item.HandType : EEquipHand::None;

		InputContents.Add(RowData);
	}

	return InputContents;
}

void UInventoryComponent::PermuteDataContentsToItemData()
{
	for (auto& Data : DataContents)
	{
		SetupDataContent(Data);
	}
}

void UInventoryComponent::RefreshExpendableIndex()
{
	if(ExpendableEquip.Num() == 0) ExpendableIndex = 0;
	else if (ExpendableEquip.Num() == 1)
	{
		for (int8 i = 0; i < 8; i++)
		{
			if(GetItem(EItemType::Expendable, i))
			{
				ExpendableIndex = i;
				break;
			}
		}
	}
	else
	{
		if (!GetItem(EItemType::Expendable, ExpendableIndex))
		{
			for (int8 i = ExpendableIndex == GetMaxExpendableIndex() ? 0 : ExpendableIndex; i < 8; i++)
			{
				if(GetItem(EItemType::Expendable, i))
				{
					ExpendableIndex = i;
					break;
				}
			}
		}
	}

	OnEquipSlotUpdated.Broadcast(EItemType::Expendable, EEquipHand::None,
		GetItem(EItemType::Expendable, ExpendableIndex) ?
		*GetItem(EItemType::Expendable, ExpendableIndex) : FInventoryData{},
		false);
	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();
}

void UInventoryComponent::RefreshSpellIndex()
{
	if (SpellEquip.Num() == 0) SpellIndex = 0;
	else if (SpellEquip.Num() == 1)
	{
		for (int8 i = 0; i < 8; i++)
		{
			if (GetItem(EItemType::Spell, i))
			{
				SpellIndex = i;
				break;
			}
		}
	}
	else
	{
		if (!GetItem(EItemType::Spell, SpellIndex))
		{
			for (int8 i = SpellIndex == GetMaxSpellIndex() ? 0 : SpellIndex; i < 8; i++)
			{
				if (GetItem(EItemType::Expendable, i))
				{
					SpellIndex = i;
					break;
				}
			}
		}
	}

	OnEquipSlotUpdated.Broadcast(EItemType::Spell, EEquipHand::None,
		GetItem(EItemType::Spell, SpellIndex) ?
		*GetItem(EItemType::Spell, SpellIndex) : FInventoryData{},
		false);
	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();
}

void UInventoryComponent::SetEquip(FInventoryData Item, bool bEquip)
{
	for (FInventoryData& ItemData : Items)
	{
		if (ItemData == Item)
		{
			ItemData.bEquip = bEquip;
			break;
			
		}
	}
}

void UInventoryComponent::SetEquipAndIndex(FInventoryData Item, bool bEquip, uint8 EquipIndex)
{
	for (FInventoryData& ItemData : Items)
	{
		if (ItemData == Item)
		{
			ItemData.bEquip = bEquip;
			ItemData.EquipIndex = EquipIndex;
			break;

		}
	}
}

void UInventoryComponent::SetEquipType(FInventoryData Item, uint8 Index, EEquipHand HandType)
{
	for (FInventoryData& ItemData : Items)
	{
		if (ItemData == Item)
		{
			ItemData.EquipIndex = Index;
			ItemData.HandType = HandType;
			break;
		}
	}
}

void UInventoryComponent::UpdatedItemCount(FInventoryData Data)
{
	if (FInventoryData* RowData = GetIndexDataFromArray(Data))
	{
		//GetIndexDataFromArray(Data)->ItemCount--;
		//Data.ItemCount--;
		RowData->ItemCount--;
		if (RowData->ItemCount == 0)
		{
			if (RowData->ItemData.ItemName != "HealthPotion" && RowData->ItemData.ItemName != "ManaPotion")
			{
				RemoveItem(*RowData);
			}
		}
	}
	if (FInventoryData * RowData = GetItem(Data.ItemData.ItemType, Data.EquipIndex))
	{
		//GetItem(EItemType::Expendable, Data.EquipIndex)->ItemCount--;
		RowData->ItemCount--;

		if (RowData->ItemCount == 0)
		{
			if (RowData->ItemData.ItemName != "HealthPotion" && RowData->ItemData.ItemName != "ManaPotion")
			{
				RemoveItem(*RowData);
			}
		}
	}
	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();

	OnEquipSlotUpdated.Broadcast(Data.ItemData.ItemType, Data.HandType, 
		GetItem(Data.ItemData.ItemType, Data.EquipIndex) ?
		*GetItem(Data.ItemData.ItemType, Data.EquipIndex, Data.HandType) : FInventoryData{},
		false);

}

void UInventoryComponent::SetupDataContent(FDataContent Data)
{
	if (!GetWorld()->GetGameInstance()->GetClass()->ImplementsInterface(UDataInterface::StaticClass()))
	{

		UE_LOG(LogSoulItem, Warning, TEXT("Not Found Interface"));
		return;
	}
	else
	{
		FInventoryData DefaultData = SearchInvenData(GetOwner(), Data, false);
		if (DefaultData.IsSet())
		{
			switch (DefaultData.ItemData.ItemType)
			{
				case EItemType::Weapon:
				{
					//DefaultData.WeaponData = (*WeaponData);
					//DefaultData.ItemCount = Data.DataCount;
					AddItem(DefaultData);

					if (!Data.bEquip || Data.HandType == EEquipHand::None || Data.EquipIndex > MaxWeaopnCount - 1) return;

					if (GetItem(DefaultData.ItemData.ItemType, Data.EquipIndex, Data.HandType)) break;
					AddEquipItem(*GetIndexDataFromArray(DefaultData), Data.EquipIndex, Data.HandType);
					//SetEquipType(DefaultData, Data.EquipIndex, Data.HandType);
				}
					return;

				case EItemType::Helmet:
				case EItemType::Armor:
				case EItemType::Pant:
				case EItemType::Boots:
				{
					
					AddItem(DefaultData);

					if (!Data.bEquip) break;
					if (!GetItem(DefaultData.ItemData.ItemType)->IsSet())
					{
						AddEquipItem(DefaultData);
					}
				}
					return;

				case EItemType::Arrow:
				case EItemType::Enhancement:
				case EItemType::Expendable:
				{
					AddItem(DefaultData);

					SoulDebug::ScreenLog(FString::Printf(TEXT("%d"), DefaultData.ItemCount), FColor::Purple, 10.0f);


					if (!Data.bEquip) break;
					if (GetItem(DefaultData.ItemData.ItemType, Data.EquipIndex)) break;
					AddEquipItem(*GetIndexDataFromArray(DefaultData), Data.EquipIndex);
				}
					return;

				case EItemType::Spell:
				{
					AddItem(DefaultData);
					if (!Data.bEquip) break;
					if (GetItem(DefaultData.ItemData.ItemType, Data.EquipIndex)) break;
					AddEquipItem(*GetIndexDataFromArray(DefaultData), Data.EquipIndex);
				}
					return;

				default:
					return;
			}
		}
	}
}

void UInventoryComponent::SetMaxPotionsCount_Implementation(uint8 MaxCount, uint8 MaxHealthCount, uint8 MaxManaCount)
{
	if(MaxCount != MaxHealthCount + MaxManaCount || MaxHealthCount > 9 || MaxManaCount > 9) return;
	
	MaxPotionCount = MaxCount;
	MaxHealthPotionCount = MaxHealthCount;
	MaxManaPotionCount = MaxManaCount;
}

void UInventoryComponent::SetPotionsData_Implementation(FDataContent HealthData, FDataContent ManaData)
{
	if(HealthData.DataCount < 0 || HealthData.DataCount > 9|| ManaData.DataCount < 0 || ManaData.DataCount > 9) return;

	HealthPotionCount = HealthData.DataCount;
	ManaPotionCount = ManaData.DataCount;

	FDataContent HealthPotion = HealthData;
	HealthPotion.DataName = TEXT("HealthPotion");

	FDataContent ManaPotion = ManaData;
	ManaPotion.DataName = TEXT("ManaPotion");

	DataContents.Add(HealthPotion);
	DataContents.Add(ManaPotion);
}
void UInventoryComponent::SetPotionsDataWithCount_Implementation(FDataContent HealthData, FDataContent ManaData, uint8 HealthCount, uint8 ManaCount)
{
	if (MaxPotionCount < HealthCount + ManaCount) return;

	HealthPotionCount = HealthCount;
	ManaPotionCount = ManaCount;

	FDataContent HealthPotion = HealthData;
	HealthPotion.DataName = TEXT("HealthPotion");
	HealthPotion.DataCount = HealthCount;

	FDataContent ManaPotion = ManaData;
	ManaPotion.DataName = TEXT("ManaPotion");
	ManaPotion.DataCount = ManaCount;

	DataContents.Add(HealthPotion);
	DataContents.Add(ManaPotion);
}

void UInventoryComponent::SetInGamePotionsCount_Implementation(int8 HealthCount, int8 ManaCount)
{
	if (HealthCount < 0 || HealthCount > 9 || ManaCount < 0 || ManaCount > 9) return;

	MaxHealthPotionCount = HealthCount;
	MaxManaPotionCount = ManaCount;

	Items[0].ItemCount = MaxHealthPotionCount;
	Items[1].ItemCount = MaxManaPotionCount;
	GetItem(EItemType::Expendable, Items[0].EquipIndex)->ItemCount = MaxHealthPotionCount;
	GetItem(EItemType::Expendable, Items[1].EquipIndex)->ItemCount = MaxManaPotionCount;


	OnEquipmentUpdated.Broadcast();
	OnInventoryUpdated.Broadcast();
	if(Items[0].bEquip && Items[0].EquipIndex == ExpendableIndex) OnEquipSlotUpdated.Broadcast(EItemType::Expendable, EEquipHand::None, Items[0], false);
	else if(Items[1].bEquip && Items[1].EquipIndex == ExpendableIndex) OnEquipSlotUpdated.Broadcast(EItemType::Expendable, EEquipHand::None, Items[1], false);
}
	
int8 UInventoryComponent::GetMaxExpendableIndex()
{
	int8 MaxKey = 0;
	for (int8 i = 0; i < 8; i++)
	{
		if (ExpendableEquip.Find(i))
		{
			if (MaxKey <= i) MaxKey = i;
		}
	}
	return MaxKey;
}

int8 UInventoryComponent::GetMaxSpellIndex()
{
	int8 MaxKey = 0;
	for (int8 i = 0; i < 4; i++)
	{
		if (SpellEquip.Find(i))
		{
			if (MaxKey <= i) MaxKey = i;
		}
	}
	return MaxKey;
}

void UInventoryComponent::RefreshItems(TArray<FInventoryData> ApplyDatas)
{
	for (auto& Data : ApplyDatas)
	{
		if (Data.ItemData.ItemType == EItemType::Expendable || Data.ItemData.ItemType == EItemType::Arrow)
		{
			if (Data.ItemCount == 0)
			{
				if(Data.bEquip) RemoveEquipItem(Data, Data.EquipIndex, Data.HandType);
				ApplyDatas.RemoveSingle(Data);
				RefreshItems(ApplyDatas);
			}
		}
	}
}

void UInventoryComponent::RemoveBasicItems(TArray<FName> RemoveItemNames)
{
	for (auto& Item : Items)
	{
		for (auto& RemoveItemName : RemoveItemNames)
		{
			if (Item.ItemData.ItemName == RemoveItemName)
			{
				Items.RemoveSingle(Item);
				RemoveBasicItems(RemoveItemNames);

				return;
			}
		}
	}
}

void UInventoryComponent::AllRefreshSlots_Implementation()
{
	//Inventory, Eqiupment//
	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();

	//if(HelmetEquip.IsSet())
	//	OnEquipSlotUpdated.Broadcast(Data.ItemData.ItemType, Data.HandType,
	//		GetItem(Data.ItemData.ItemType, Data.EquipIndex) ?
	//		*GetItem(Data.ItemData.ItemType, Data.EquipIndex, Data.HandType) : FInventoryData{},
	//		false);
	//if(ArmorEquip.IsSet())
	//	OnEquipSlotUpdated.Broadcast(Data.ItemData.ItemType, Data.HandType,
	//		GetItem(Data.ItemData.ItemType, Data.EquipIndex) ?
	//		*GetItem(Data.ItemData.ItemType, Data.EquipIndex, Data.HandType) : FInventoryData{},
	//		false);

	//Slot//
	OnEquipSlotUpdated.Broadcast(EItemType::Spell, EEquipHand::None, 
			GetItem(EItemType::Spell, SpellIndex) ?
			*GetItem(EItemType::Spell, SpellIndex) : FInventoryData{},
			false);
	OnEquipSlotUpdated.Broadcast(EItemType::Expendable, EEquipHand::None,
		GetItem(EItemType::Expendable, ExpendableIndex) ?
		*GetItem(EItemType::Expendable, ExpendableIndex) : FInventoryData{},
		false);
	OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Left,
		GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) ?
		*GetItem(EItemType::Weapon, L_AttachIndex, EEquipHand::Left) : FInventoryData{},
		false);
	OnEquipSlotUpdated.Broadcast(EItemType::Weapon, EEquipHand::Right,
		GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) ?
		*GetItem(EItemType::Weapon, R_AttachIndex, EEquipHand::Right) : FInventoryData{},
		false);
}

void UInventoryComponent::AddMaxPotionCount_Implementation()
{
	if (MaxPotionCount == MaxHealthPotionCount + MaxManaPotionCount)
	{
		GetPotionUpgradeItem()->ItemCount -= 1;
		MaxPotionCount += 1;
		MaxHealthPotionCount += 1;
		Items[0].ItemCount = MaxHealthPotionCount;
		GetItem(EItemType::Expendable, Items[0].EquipIndex)->ItemCount = MaxHealthPotionCount;



		for (auto& Item : Items)
		{
			if (Item.ItemData.ItemType == EItemType::Expendable || 
				Item.ItemData.ItemType == EItemType::Arrow || 
				Item.ItemData.ItemType == EItemType::Enhancement)
			{
				if (Item.ItemCount == 0)
				{
					if (Item.bEquip) RemoveEquipItem(Item, Item.EquipIndex, Item.HandType);
					Items.RemoveSingle(Item);
					RefreshItems(Items);
				}
			}
		}

		AllRefreshSlots();
	}
}

void UInventoryComponent::SetupPotion()
{
	//for (auto& Item : Items)
	//{
	//	for (auto Potion : Potions)
	//	{
	//		if (Item.ItemData.ItemName == Potion)
	//		{
	//			if (Potion == TEXT("HealthPotion"))
	//			{
	//				Item.ItemCount = HealthPotionCount; GetItem(EItemType::Expendable, Item.EquipIndex)->ItemCount = HealthPotionCount;
	//			}
	//			else if (Potion == TEXT("ManaPotion"))
	//			{
	//				Item.ItemCount = ManaPotionCount; GetItem(EItemType::Expendable, Item.EquipIndex)->ItemCount = ManaPotionCount;
	//			}
	//
	//
	//			Potions.RemoveSingle(Potion);
	//		}
	//	}
	//	if (Potions.Num() == 0)
	//	{
	//		break;
	//	}
	//}

	Items[0].ItemCount = HealthPotionCount;
	Items[1].ItemCount = ManaPotionCount;
	GetItem(EItemType::Expendable, Items[0].EquipIndex)->ItemCount = HealthPotionCount;
	GetItem(EItemType::Expendable, Items[1].EquipIndex)->ItemCount = ManaPotionCount;

	OnInventoryUpdated.Broadcast();
	OnEquipmentUpdated.Broadcast();
	OnEquipSlotUpdated.Broadcast(EItemType::Expendable, EEquipHand::None, 
		GetItem(EItemType::Expendable, ExpendableIndex) ?
		*GetItem(EItemType::Expendable, ExpendableIndex) : FInventoryData{},
		false);
}
