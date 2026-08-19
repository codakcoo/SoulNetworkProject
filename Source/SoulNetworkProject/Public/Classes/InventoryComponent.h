// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulNetworkProject/ItemData.h"
#include "SoulNetworkProject/ItemState.h"
#include "InventoryComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewListUpdated, FInventoryData, ItemData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnEquipSlotUpdated, EItemType, SlotType, EEquipHand, HandType, FInventoryData, ItemData, bool, bAnim);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULNETWORKPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	bool AddItem(FInventoryData& Item, bool bShowList = false);
	UFUNCTION()
	bool RemoveItem(FInventoryData& Item);

	UFUNCTION(Client , Reliable, WithValidation)
	void Client_AddItem(FInventoryData Item, bool bShowList = false);

	UFUNCTION()
	void UseItem(FInventoryData& Item);
	UFUNCTION()
	void DropItem(FInventoryData& Item, uint8 DropCount = 0);
	UFUNCTION()
	void DestroyItem(FInventoryData& Item, uint8 DestroyCount = 0);

	UFUNCTION()
	void DropAtOnceItem(TArray<FInventoryData> DropItems);
	UFUNCTION()
	void DestroyAtOnceItem(TArray<FInventoryData> DestroyItems);


	UFUNCTION()
	bool AddEquipItem(FInventoryData& Item, int32 Index = 0, EEquipHand HandType = EEquipHand::None);
	UFUNCTION()
	bool RemoveEquipItem(FInventoryData& Item, int32 Index = 0, EEquipHand HandType = EEquipHand::None);

	FInventoryData* GetItem(EItemType ItemType, uint8 Index = 0, EEquipHand HandType = EEquipHand::None);
	FInventoryData* GetIndexDataFromArray(FInventoryData SearchData);

	FInventoryData* GetCountItem(FName DataName);
	FInventoryData* GetItemForSameName(FName DataName);

	void SetSpellIndex(uint8 Index);
	void SetExpendableIndex(uint8 Index);
	void SetL_AttachIndex(uint8 Index);
	void SetR_AttachIndex(uint8 Index);

	FORCEINLINE uint8 GetSpellIndex() const { return SpellIndex; }
	FORCEINLINE uint8 GetExpendableIndex() const { return ExpendableIndex; }
	FORCEINLINE uint8 GetL_AttachIndex() const { return L_AttachIndex; }
	FORCEINLINE uint8 GetR_AttachIndex() const { return R_AttachIndex; }

	UFUNCTION()
	uint8 GetMaxPotionCount() const { return MaxPotionCount; }
	UFUNCTION()
	uint8 GetMaxHealthCount() const { return MaxHealthPotionCount; }
	UFUNCTION()
	uint8 GetMaxManaCount() const { return MaxManaPotionCount; }
	UFUNCTION()
	bool CanAddCountByPotions() const { return MaxPotionCount == 12 ? false : true; }
	FInventoryData* GetPotionUpgradeItem();

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_RefreshExpendableItem(class APlayerCharacter* CalledOwner, FInventoryData Data);


	UFUNCTION(BlueprintCallable)
	void SwitchLeftWeapon();
	UFUNCTION(BlueprintCallable)
	void SwitchRightWeapon();
	UFUNCTION(BlueprintCallable)
	void SwitchExpendable();
	UFUNCTION(BlueprintCallable)
	void SwitchSpell();

	UFUNCTION()
	void UpdatedItemCount(FInventoryData Data);



	UFUNCTION()
	void SetupDataContent(FDataContent Data);
	UFUNCTION(Client, Reliable)
	void SetMaxPotionsCount(uint8 MaxCount, uint8 MaxHealthCount, uint8 MaxManaCount);
	UFUNCTION(Client, Reliable)
	void SetPotionsData(FDataContent HealthData, FDataContent ManaData);
	UFUNCTION(Client, Reliable)
	void SetPotionsDataWithCount(FDataContent HealthData, FDataContent ManaData, uint8 HealthCount, uint8 ManaCount);
	UFUNCTION(Client, Reliable)
	void SetInGamePotionsCount(int8 HealthCount, int8 ManaCount);
	UFUNCTION(Client, Reliable)
	void AllRefreshSlots();
	UFUNCTION(Client, Reliable)
	void AddMaxPotionCount();

	UFUNCTION()
	void SetupPotion();
	UFUNCTION()
	void SetDataContents(TArray<FDataContent>& InContents) { DataContents = InContents; }
	UFUNCTION(Client, Reliable)
	void SetData(const TArray<FDataContent>& InDataContents);

	FDataContent ChangeItemDataToContent(FInventoryData ItemData);
	UFUNCTION()
	TArray<FDataContent> PermuteItemDataToContents();
	UFUNCTION()
	void PermuteDataContentsToItemData();

	UFUNCTION()
	void RefreshExpendableIndex();
	UFUNCTION()
	void RefreshSpellIndex();

private:
	UFUNCTION()
	void SetEquip(FInventoryData Item, bool bEquip);
	UFUNCTION()
	void SetEquipAndIndex(FInventoryData Item, bool bEquip, uint8 EquipIndex);
	UFUNCTION()
	void SetEquipType(FInventoryData Item, uint8 Index, EEquipHand HandType);


	//true�� 0���ų� ������ �������̰�, false�� �� �� �ش� ����.//
	UFUNCTION()
	int8 GetMaxExpendableIndex();
	UFUNCTION()
	int8 GetMaxSpellIndex();
	
	UFUNCTION()
	void RefreshItems(TArray<FInventoryData> ApplyDatas);

	UFUNCTION()
	void RemoveBasicItems(TArray<FName> RemoveItemNames);

public:

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentUpdated OnEquipmentUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Slot")
	FOnEquipSlotUpdated OnEquipSlotUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Slot")
	FOnViewListUpdated OnViewListUpdated;

	/********************************Inventory********************************************/
	UPROPERTY(EditDefaultsOnly)
	TArray<FDataContent> DataContents;

	UPROPERTY()
	TArray<FInventoryData> Items;


	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Capacity;
	/********************************Equipment********************************************/
	UPROPERTY()
	TMap<uint8, FInventoryData> L_WeaponEquip;
	UPROPERTY()
	TMap<uint8, FInventoryData> R_WeaponEquip;

	UPROPERTY()
	FInventoryData HelmetEquip;
	UPROPERTY()
	FInventoryData ArmorEquip;
	UPROPERTY()
	FInventoryData PantEquip;
	UPROPERTY()
	FInventoryData BootsEquip;

	UPROPERTY()
	FInventoryData FirstArrowEquip;
	UPROPERTY()
	FInventoryData SecondArrowEquip;

	UPROPERTY()
	TMap<uint8, FInventoryData> ArrowEquip;

	UPROPERTY()
	TMap<uint8, FInventoryData> ExpendableEquip;

	UPROPERTY()
	TMap<uint8, FInventoryData> SpellEquip;

	/////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, Category = "Value")
	uint8 MaxWeaponIndex = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Value")
	uint8 MaxSpellIndex = 4;
	UPROPERTY(EditDefaultsOnly, Category = "Value")
	uint8 MaxExpendableIndex = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Value")
	uint8 MaxWeaopnCount = 8;
	UPROPERTY(EditDefaultsOnly, Category = "Value")
	uint8 MaxClothCount = 5;
	UPROPERTY(EditDefaultsOnly, Category = "Value")
	uint8 MaxExpendableCount = 99;

private:
	class APlayerCharacter* OwnerPawn;

	uint8 L_AttachIndex = 0;
	uint8 R_AttachIndex = 0;

	uint8 ExpendableIndex = 0;
	uint8 SpellIndex = 0;

	uint8 MaxPotionCount = 0;
	uint8 MaxHealthPotionCount = 0;
	uint8 MaxManaPotionCount = 0;

	uint8 HealthPotionCount = 0;
	uint8 ManaPotionCount = 0;
};

template<typename T>
uint8 GetItemEnumValue(T CheckData)
{
#if !WITH_EDITOR
	return 0;
#else
	TOptional<uint8> OutputValue;


	switch (CheckData.ItemData.ItemType)
	{
	case EItemType::Weapon:
		return (uint8)CheckData.WeaponData.WeaponType;
	case EItemType::Helmet:
	case EItemType::Armor:
	case EItemType::Pant:
	case EItemType::Boots:
		return (uint8)CheckData.ClothData.ClothType;
	case EItemType::Expendable:
		return (uint8)CheckData.ExpendableData.ExpendableType;
	case EItemType::Arrow:
		return 2;
	case EItemType::Spell:
		return (uint8)CheckData.SpellData.SpellType;
	default:
		break;
	}

#pragma warning(disable : 4996)
	return 0;
#endif
}