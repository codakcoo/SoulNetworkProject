#pragma once


#include "SoulNetworkProject/ItemState.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Classes/Magic/MasterMagicActor.h"
#include "Interface/DataInterface.h"
#include "SoulNetworkLog.h"
#include "ItemData.generated.h"

//Content Data//
USTRUCT(BlueprintType)
struct FDataContent
{
	GENERATED_USTRUCT_BODY()

public:
	//자신이 가져올 테이터의 이름을 정확히 가져와야함.//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DataName;

	//장비 일경우 아이템 레벨, 소비품 일경우 아이템 갯수임. (무기 : 0 ~ 8레벨, 옷 : 0 ~ 5레벨, 소비품 : 99개 까지.)//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 DataCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEquip = false;

	//해당 인덱스에 아이템이 존재할경우 차례대로 상속됨.//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEquip == true"))
	uint8 EquipIndex = 0;
	//자신이 지정해도 되며 None으로 지정시 그냥 장착이 안되며 그냥 인벤토리에 상속됨.//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bEquip == true"))
	EEquipHand HandType;

	FDataContent() {}
	~FDataContent() {}
};
/////////////////

//Item Data//
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		bool bUseOption = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		bool bDropOption = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		bool bDestroyOption = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		bool bDropAtOnceOption = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		bool bDestroyAtOnceOption = false;

	void ResetValue()
	{
		//ItemClass = nullptr;
		ItemType = EItemType::All;
		ItemName = NAME_None;

		ItemImage = nullptr;
		Mesh = nullptr;

		bUseOption = false;
		bDropOption = false;
		bDestroyOption = false;
		bDropAtOnceOption = false;
		bDestroyAtOnceOption = false;
	}
	bool IsSet()
	{
		bool bIsSet = false;

		if (ItemType != EItemType::All && ItemName != "" && ItemImage != nullptr)
		{
			bIsSet = true;
		}

		return bIsSet;
	}

	bool operator == (const FItemData& rhs) const
	{
		if (//ItemClass == rhs.ItemClass 
			ItemType == rhs.ItemType
			&& ItemName.IsEqual(rhs.ItemName)
			&& ItemImage == rhs.ItemImage
			&& Mesh == rhs.Mesh
			&& bUseOption == rhs.bUseOption
			&& bDropOption == rhs.bDropOption
			&& bDestroyOption == rhs.bDestroyOption
			&& bDropAtOnceOption == rhs.bDropAtOnceOption
			&& bDestroyAtOnceOption == rhs.bDestroyAtOnceOption)
		{
			return true;
		}
		return false;
	}
};
///////////////

//Weapon Data//
USTRUCT(BlueprintType)
struct FDamageStat
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float STRStat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float INTStat;

	bool operator ==(const FDamageStat& rhs)
	{
		if (STRStat == rhs.STRStat
			&& INTStat == rhs.INTStat)
		{
			return true;
		}
		return false;
	}
	bool operator !=(const FDamageStat& rhs)
	{
		if (STRStat != rhs.STRStat
			|| INTStat != rhs.INTStat)
		{
			return true;
		}
		return false;
	}
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ManaCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TraceSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDamageStat> DamageStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 MaxLowAttackCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 MaxHeavyAttackCount;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* LowAttackMt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimMontage* AOEMt;

	FWeaponData()
	{
		ManaCost = 0.f;
		TraceSize = 15.f;

		for (int8 i = 0; i < 9; i++)
		{
			FDamageStat AddArr;
			AddArr.STRStat = 0.f;
			AddArr.INTStat = 0.f;
			DamageStat.Add(AddArr);
		}

		MaxLowAttackCount = 2;
		MaxHeavyAttackCount = 2;

		LowAttackMt = nullptr;
		AOEMt = nullptr;
	}

	void ResetValue()
	{
		WeaponType = EWeaponType::Strike;

		ManaCost = 0.f;
		MaxLowAttackCount = 0;
		MaxHeavyAttackCount = 0;

		MaxLowAttackCount = 0;
		MaxHeavyAttackCount = 0;

		LowAttackMt = nullptr;
		AOEMt = nullptr;
		DamageStat.Empty();
	}

	bool IsSet()
	{
		bool bIsSet = false;

		if (ManaCost > 0.f && MaxLowAttackCount > 0 && LowAttackMt)// || AOEMt)// || MaxHeavyAttackCount > 0) //뒤 주석들은 정의 필요//
		{
			bIsSet = true;
		}

		return bIsSet;
	}

	bool operator ==(const FWeaponData& rhs)
	{
		if (ManaCost == rhs.ManaCost
			&& LowAttackMt == rhs.LowAttackMt
			&& AOEMt == rhs.AOEMt
			&& MaxLowAttackCount == rhs.MaxLowAttackCount
			&& MaxHeavyAttackCount == rhs.MaxHeavyAttackCount
			&& WeaponType == rhs.WeaponType)
		{
			for (int8 i = 0; i < DamageStat.Num(); i++)
			{
				if (DamageStat[i] != rhs.DamageStat[i]) return false;
			}
			return true;
		}
		return false;
	}
};
////////////////

//Cloth Data//
USTRUCT(BlueprintType)
struct FDefenseStat
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Physics = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Magic = 0.f;

	bool operator ==(const FDefenseStat& rhs)
	{
		if (Physics == rhs.Physics
			&& Magic == rhs.Magic)
		{
			return true;
		}
		return false;
	}
	bool operator !=(const FDefenseStat& rhs)
	{
		if (Physics != rhs.Physics
			|| Magic != rhs.Magic)
		{
			return true;
		}
		return false;
	}
};

USTRUCT(BlueprintType)
struct FClothData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EClothType ClothType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDefenseStat> DefenseStat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<EBodyType> BodyTypes;

	FClothData()
	{
		ClothType = EClothType::Stitch;
		Weight = 10.f;

		for (int8 i = 0; i < 6; i++)
		{
			FDefenseStat AddArr;
			AddArr.Physics = 0.f;
			AddArr.Magic = 0.f;

			DefenseStat.Add(AddArr);
		}
	}

	void ResetValue()
	{
		Weight = 0.f;

		DefenseStat.Empty();
		BodyTypes.Empty();
	}

	bool IsSet()
	{
		bool bIsSet = false;

		if (Weight > 0.f)
		{
			bIsSet = true;
		}

		return bIsSet;
	}

	bool operator ==(const FClothData& rhs)
	{
		if (Weight == rhs.Weight && DefenseStat.Num() == rhs.DefenseStat.Num() && BodyTypes.Num() == rhs.BodyTypes.Num())
		{
			for (int8 i = 0; i < DefenseStat.Num(); i++)
			{
				if (DefenseStat[i] != rhs.DefenseStat[i]) return false;
			}
			for (int8 i = 0; i < BodyTypes.Num(); i++)
			{
				if (BodyTypes[i] != rhs.BodyTypes[i]) return false;
			}
			return true;
		}
		return false;
	}

	bool operator !=(const FClothData& rhs)
	{
		if (Weight != rhs.Weight || DefenseStat.Num() != rhs.DefenseStat.Num() || BodyTypes.Num() != rhs.BodyTypes.Num())
		{
			return true;
		}
		else
		{
			for (int8 i = 0; i < DefenseStat.Num(); i++)
			{
				if (DefenseStat[i] == rhs.DefenseStat[i]) return false;
			}
			for (int8 i = 0; i < BodyTypes.Num(); i++)
			{
				if (BodyTypes[i] == rhs.BodyTypes[i]) return false;
			}
		}
		return true;
	}
};
/////////////////

//Expendable Data//
USTRUCT(BlueprintType)
struct FExpendableData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EExpendableType ExpendableType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EExpendableElementType ElementType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IncreaseValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* ReactionMt;

	FExpendableData()
	{
		ExpendableType = EExpendableType::Potion;
		IncreaseValue = 0;
		ReactionMt = nullptr;
	}

	void ResetValue()
	{
		ExpendableType = EExpendableType::Potion;
		IncreaseValue = 0;
		ReactionMt = nullptr;
	}

	bool IsSet()
	{
		bool bIsSet = false;

		if (IncreaseValue > 0.f)// && ReactionMt)
		{
			if(ExpendableType == EExpendableType::Tool) bIsSet = true;
			else ReactionMt ? bIsSet = true : false;
			bIsSet = true;
		}

		return bIsSet;
	}

	bool operator ==(const FExpendableData& rhs)
	{
		if(ExpendableType == rhs.ExpendableType
			&& IncreaseValue == rhs.IncreaseValue
			&& ReactionMt == rhs.ReactionMt)
		{
			return true;
		}
		return false;
	}
	bool operator !=(const FExpendableData& rhs)
	{
		if (ExpendableType != rhs.ExpendableType
			|| IncreaseValue != rhs.IncreaseValue
			|| ReactionMt != rhs.ReactionMt)
		{
			return true;
		}
		return false;
	}
};
////////////////////

//Spell Data//
USTRUCT(BlueprintType)
struct FSpellData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	ESpellType SpellType;
	UPROPERTY(EditAnywhere)
	ESpellMotionType MotionType;
	UPROPERTY(EditAnywhere)
	uint8 ComboIndex;
	UPROPERTY(EditAnywhere)
	float ManaCost = 0.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMasterMagicActor> MagicClass;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* SpellMotion;

	FSpellData()
	{
		SpellType = ESpellType::None;
		MagicClass = nullptr;
		SpellMotion = nullptr;
	}

	void ResetValue()
	{
		SpellType = ESpellType::None;
		MagicClass = nullptr;
		SpellMotion = nullptr;
	}

	bool IsSet()
	{
		bool bIsSet = false;

		if (SpellMotion != nullptr && MagicClass != nullptr)
		{
			bIsSet = true;
		}

		return bIsSet;
	}

	bool operator ==(const FSpellData& rhs)
	{
		if(rhs.SpellType != ESpellType::None)
		{
			if (SpellType == rhs.SpellType
				&& SpellMotion == rhs.SpellMotion
				&& MagicClass == rhs.MagicClass)
			{
				return true;
			}
		}
		return false;
	}
	bool operator !=(const FSpellData& rhs)
	{
		if(rhs.SpellType == ESpellType::None) return false;

		if (SpellType != rhs.SpellType
			|| SpellMotion != rhs.SpellMotion
			|| MagicClass != rhs.MagicClass)
		{
			return true;
		}
		return false;
	}
};
////////////////

USTRUCT(BlueprintType)
struct FInventoryData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FItemData ItemData;
	UPROPERTY()
	FWeaponData WeaponData;
	UPROPERTY()
	FClothData ClothData;
	UPROPERTY()
	FExpendableData ExpendableData;
	UPROPERTY()
	FSpellData SpellData;


	UPROPERTY()
	bool bEquip = false;
	UPROPERTY()
	uint8 EquipIndex;
	UPROPERTY()
	EEquipHand HandType = EEquipHand::None;

	UPROPERTY()
	uint32 StackIndex;
	UPROPERTY()
	int32 ItemCount = 0;

	FInventoryData() {}
	~FInventoryData() {}

	void PermuteValue(FInventoryData& PermuteData)
	{
		ItemCount = PermuteData.ItemCount;
		bEquip = PermuteData.bEquip;
		EquipIndex = PermuteData.EquipIndex;
		StackIndex = PermuteData.StackIndex;
		HandType = PermuteData.HandType;

		ItemData = PermuteData.ItemData;
		WeaponData = PermuteData.WeaponData;
		ClothData = PermuteData.ClothData;
		ExpendableData = PermuteData.ExpendableData;
		SpellData = PermuteData.SpellData;
	}

	void ResetValue()
	{
		ItemData.ResetValue();
		ClothData.ResetValue();
		WeaponData.ResetValue();
		ExpendableData.ResetValue();
		SpellData.ResetValue();

		bEquip = false;
		EquipIndex = 0;
		HandType = EEquipHand::None;

		StackIndex = 0;
		ItemCount = 0;
	}

	bool IsSet()
	{
		if(!ItemData.IsSet())
		{
			//if(ItemData.ItemType == EItemType::Weapon)
			//{
			//	bIsSet = WeaponData.IsSet();
			//}
			//else if (ItemData.ItemType == EItemType::Expendable)
			//{
			//	bIsSet = ExpendableData.IsSet();
			//}
			//else
			//{
			//	bIsSet = ClothData.IsSet();
			//}
			return false;
		}
		return true;
	}


	bool operator == (const FInventoryData& rhs) const
	{
		if (ItemData == rhs.ItemData
			// ItemCount == rhs.ItemCount
			&& StackIndex == rhs.StackIndex)
		{

			return true;

		}
		return false;
	}
};

//DataTable Row찾기//
template<class T>
T* FindRow(UDataTable* DataTableAsset, FName RowName, const FString& ContextString, bool bWarnIfRowMissing = true)
{
	if (!DataTableAsset)
	{
		UE_LOG(LogSoulItem, Error, TEXT("Not have DataTable."));
		return nullptr;
	}
	else
	{
		//static const FString ContextString(TEXT("Data Context"));
		T* RowData = DataTableAsset->FindRow<T>(RowName, ContextString, true);

		if (RowData)
		{
			UE_LOG(LogSoulItem, Error, TEXT("Find RowData have %s."), *ContextString);
			return RowData;
		}
		else
		{
			UE_LOG(LogSoulItem, Error, TEXT("Not find RowData have %s."), *ContextString);
			return nullptr;
		}
	}
}

//255라고 로그가 뜨면 데이터를 찾지 못함.//
template<typename T>
uint8 GetItemEnumValue(FInventoryData& CheckData)
{
#if !WITH_EDITOR
	return 0;
#else
	TOptional<uint8> OutputValue;

	//if(CheckData.ItemData.ItemType == EItemType::Weapon)
	//	OutputValue.Emplace(static_cast<uint8>(CheckData.WeaponData.WeaponType));
	//
	//if (CheckData.ItemData.ItemType == EItemType::Helmet)
	//	OutputValue.Emplace(static_cast<uint8>(CheckData.ClothData.ClothType));
	//if (CheckData.ItemData.ItemType == EItemType::Armor)
	//	OutputValue.Emplace(static_cast<uint8>(CheckData.ClothData.ClothType));
	//if (CheckData.ItemData.ItemType == EItemType::Pant)
	//	OutputValue.Emplace(static_cast<uint8>(CheckData.ClothData.ClothType));
	//if (CheckData.ItemData.ItemType == EItemType::Boots)
	//	OutputValue.Emplace(static_cast<uint8>(CheckData.ClothData.ClothType));
	//
	//if (CheckData.ItemData.ItemType == EItemType::Expendable)
	//	OutputValue.Emplace(static_cast<uint8>(CheckData.ExpendableData.ExpendableType));
	//if (CheckData.ItemData.ItemType == EItemType::Arrow)
	//	OutputValue.Emplace(static_cast<uint8>(2));
	//if (CheckData.ItemData.ItemType == EItemType::Spell)
	//	OutputValue.Emplace(static_cast<uint8>(CheckData.SpellData.SpellType));

	switch (CheckData.ItemData.ItemType)
	{
		case EItemType::Weapon:
		{
			OutputValue.Emplace(static_cast<uint8>(CheckData.WeaponData.WeaponType));
		}
		break;
		case EItemType::Helmet:
		case EItemType::Armor:
		case EItemType::Pant:
		case EItemType::Boots:
		{
			OutputValue.Emplace(static_cast<uint8>(CheckData.ClothData.ClothType));
		}
		break;
		case EItemType::Expendable:
		{
			OutputValue.Emplace(static_cast<uint8>(CheckData.ExpendableData.ExpendableType));
		}
		break;
		case EItemType::Arrow:
		{
			OutputValue.Emplace(static_cast<uint8>(2));
		}
		break;
		case EItemType::Spell:
		{
			OutputValue.Emplace(static_cast<uint8>(CheckData.SpellData.SpellType));
		}
		break;
	default:
		break;
	}

#pragma warning(disable : 4996)
	return OutputValue.IsSet() ? OutputValue.GetValue() : 0;
#endif
}

template<class T>
FInventoryData SearchInvenData(T* Owner, FDataContent Content, bool bResetHT)
{
	FInventoryData OutputData;
	if(!Cast<AActor>(Owner))
	{
		UE_LOG(LogSoulItem, Warning, TEXT("Not lower actor"));
		return OutputData;
	}

	IDataInterface* Interface = Cast<IDataInterface>(Owner->GetWorld()->GetGameInstance());
	if (!Interface)
	{
		UE_LOG(LogSoulItem, Warning, TEXT("Not Found Interface"));
		return OutputData;
	}
	else
	{


		if (FindRow<FItemData>(Interface->GetItemDataTable(), Content.DataName, "Item Data Context.") == nullptr) return OutputData;

		OutputData.ItemData = (*FindRow<FItemData>(Interface->GetItemDataTable(), Content.DataName, "Item Data Context."));
		switch (OutputData.ItemData.ItemType)
		{
			case EItemType::Weapon:
			{
				if (Content.DataCount < 0 || Content.DataCount > 8) OutputData.ResetValue();
				else if (FWeaponData* WeaponData = FindRow<FWeaponData>(Interface->GetWeaponDataTable(), Content.DataName, "Weapon Data Context."))
				{

					OutputData.WeaponData = (*WeaponData);
					OutputData.ItemCount = Content.DataCount;
					OutputData.HandType = bResetHT ? EEquipHand::None : Content.HandType;
				}
			}
			break;

			case EItemType::Helmet:
			case EItemType::Armor:
			case EItemType::Pant:
			case EItemType::Boots:
			{
				if (Content.DataCount < 0 || Content.DataCount > 5) OutputData.ResetValue();
				if (FClothData* ClothData = FindRow<FClothData>(Interface->GetClothDataTable(), Content.DataName, "Cloth Data Context."))
				{
					OutputData.ClothData = (*ClothData);
					OutputData.ItemCount = Content.DataCount;
					OutputData.HandType = EEquipHand::None;
				}
			}
			break;
			case EItemType::Arrow:
			{
				if (FExpendableData* ExpendableData = FindRow<FExpendableData>(Interface->GetExpendableDataTable(), Content.DataName, "Expendable Data Context."))
				{

					if (Content.DataCount > 99)  OutputData.ResetValue();
					else
					{
						OutputData.ExpendableData = (*ExpendableData);
						OutputData.ItemCount = Content.DataCount;
						OutputData.HandType = EEquipHand::None;
					}
				}
			}
			break;
			case EItemType::Enhancement:
			case EItemType::Expendable:
			{
				if (FExpendableData* ExpendableData = FindRow<FExpendableData>(Interface->GetExpendableDataTable(), Content.DataName, "Expendable Data Context."))
				{
					//uint8 Index;

					if ((OutputData.ItemData.ItemType == EItemType::Expendable &&
						OutputData.ExpendableData.ExpendableType != EExpendableType::Potion) ||
						OutputData.ItemData.ItemType != EItemType::Expendable)
					{
						if (Content.DataCount > 99)  OutputData.ResetValue();

					}
					else
					{
						OutputData.ExpendableData = (*ExpendableData);
						OutputData.ItemCount = Content.DataCount;
						OutputData.HandType = EEquipHand::None;
					}

				}
			}
			break;
			//{
			//	if (FExpendableData* ExpendableData = FindRow<FExpendableData>(Interface->GetExpendableDataTable(), Content.DataName, "Expendable Data Context."))
			//	{
			//		OutputData.ExpendableData = (*ExpendableData);
			//		OutputData.ItemCount = 0;
			//		OutputData.HandType = EEquipHand::None;
			//	}
			//}
			//break;
			case EItemType::Spell:
			{
				if (FSpellData* RowSpellData = FindRow<FSpellData>(Interface->GetSpellDataTable(), Content.DataName, "Spell Data Context."))
				{
					if (Content.DataCount != 0) OutputData.ResetValue();

					OutputData.SpellData = (*RowSpellData);
					OutputData.ItemCount = 0;
					OutputData.HandType = EEquipHand::None;
				}
			}
			break;
		}
	}
	return OutputData;
}