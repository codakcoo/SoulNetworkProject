#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Class.h"
#include "Runtime/Core/Public/HAL/Platform.h"
#include "Containers/UnrealString.h"
#include "SoulNetworkLog.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	All			= 0	UMETA(DisplayName = "All"),
	Expendable	= 1	UMETA(DisplayName = "Expendable"),
	Weapon		= 2	UMETA(DisplayName = "Weapon"),
	Helmet		= 3	UMETA(DisplayName = "Helmet"),
	Armor		= 4	UMETA(DisplayName = "Armor"),
	Pant		= 5	UMETA(DisplayName = "Pant"),
	Boots		= 6	UMETA(DisplayName = "Boots"),
	Spell		= 7	UMETA(DisplayName = "Spell"),
	Arrow		= 8	UMETA(DisplayName = "Arrow"),
	Enhancement	= 9 UMETA(DisplayName = "Enhancement"),
};

UENUM(BlueprintType)
enum class ESwordType : uint8
{
	Sword				UMETA(DisplayName = "Sword"),
	GreateSword         UMETA(DisplayName = "GreateSword"),
	ShotrSword          UMETA(DisplayName = "ShotrSword")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Strike		UMETA(DisplayName = "Strike"),
	Wand		UMETA(DisplayName = "Wand"),
	Shield		UMETA(DisplayName = "Shield"),
	Bow			UMETA(DisplayName = "Bow"),
	Talisman	UMETA(DisplayName = "Talisman")
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	Hat					UMETA(DisplayName = "Hat"),
	Cheast				UMETA(DisplayName = "Cheast"),
	Pant				UMETA(DisplayName = "Pant"),
	Boot				UMETA(DisplayName = "Boot"),
	Shoulder			UMETA(DisplayName = "Shoulder"),
	Cloak				UMETA(DisplayName = "Cloak"),
	Glove				UMETA(DisplayName = "Glove"),
	Bracer				UMETA(DisplayName = "Bracer")
};

UENUM(BlueprintType)
enum class EClothType : uint8
{
	Stitch				UMETA(DisplayName = "Stitch"),
	Leather				UMETA(DisplayName = "Leather"),
	Chain				UMETA(DisplayName = "Chain"),
	Sheeting			UMETA(DisplayName = "Sheeting")
};

UENUM(BlueprintType)
enum class EEquipHand : uint8
{
	None			UMETA(DisplayName = "None"),
	Left			UMETA(DisplayName = "Left"),
	Right			UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class EBodyType : uint8
{
	BODY_HEAD			UMETA(DisplayName = "BODY_HEAD"),
	BODY_CHEST			UMETA(DisplayName = "BODY_CHEST"),
	BODY_ARMS			UMETA(DisplayName = "BODY_ARMS"),
	BODY_HANDS			UMETA(DisplayName = "BODY_HANDS"),
	BODY_LEDGS			UMETA(DisplayName = "BODY_LEDGS"),
	BODY_FEETS			UMETA(DisplayName = "BODY_FEETS")
};

UENUM(BlueprintType)
enum class EExpendableType : uint8
{
	Potion			UMETA(DisplayName = "Potion"),
	Tool			UMETA(DisplayName = "Tool"),
	Throwable		UMETA(DisplayName = "Throwable"),
	Spell			UMETA(DisplayName = "Spell"),
	Enforce			UMETA(DisplayName = "Enforce")
};

UENUM(BlueprintType)
enum class EExpendableElementType : uint8
{
	Health		UMETA(DisplayName = "Health"),
	Mana		UMETA(DisplayName = "Mana"),
	STR			UMETA(DisplayName = "STR"),
	INT			UMETA(DisplayName = "INT"),
	Fire		UMETA(DisplayName = "Fire"),
	Electric	UMETA(DisplayName = "Electric"),
	Ice			UMETA(DisplayName = "Ice")
};

UENUM(BlueprintType)
enum class ESpellType : uint8
{
	None				= 0		UMETA(DisplayName = "None"),
	AttackSorcery		= 1		UMETA(DisplayName = "AttackSorcery"),
	AssistSorcery		= 2		UMETA(DisplayName = "AssistSorcery"),
	AttackIncantation	= 3		UMETA(DisplayName = "AttackIncantation"),
	AssistIncantation	= 4		UMETA(DisplayName = "AssistIncantation")
};
UENUM(BlueprintType)
enum class ESpellMotionType : uint8
{
	Combo		UMETA(DisplayName = "Combo"),
	Repeat		UMETA(DisplayName = "Repeat"),
	Charge		UMETA(DisplayName = "Charge")
};

template<typename T>
FString EnumToString(T Enum)
{
#if !WITH_EDITOR
	return "";
#else
	static FString EnumName;
	static bool bEnumNameInit = false;

	// Init it once
	if (bEnumNameInit == false)
	{
		bEnumNameInit = true;

		FString FunctionSignature(__FUNCSIG__);
		UE_LOG(LogSoulItem, Display, TEXT("Enum funcsig: [%s]"), *FunctionSignature);

		int32 FirstIDX = FunctionSignature.Find(TEXT("(enum "), ESearchCase::CaseSensitive, ESearchDir::FromEnd);

		if (FirstIDX != INDEX_NONE)
		{
			FirstIDX = FirstIDX + 6;
			int32 LastIDX = (FunctionSignature.Len() - 1);

			if (FunctionSignature.IsValidIndex(FirstIDX) && FunctionSignature.IsValidIndex(LastIDX) && FirstIDX < LastIDX)
			{
				EnumName = FunctionSignature.Mid(FirstIDX, LastIDX - FirstIDX);
				UE_LOG(LogSoulItem, Display, TEXT("Enum name: [%s]"), *EnumName);
			}
		}
	}

	if (EnumName.IsEmpty())
		return FString("ENUM_Invalid");

	// const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, (const TCHAR *)(typeid(T).name()), true);
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!EnumPtr)
		return FString("ENUM_Invalid");

#pragma warning(disable: 4996)
	return EnumPtr->GetDisplayNameText(static_cast<int32>(Enum)).ToString();
#endif
}

template<typename T>
UEnum* GetEnumObject(T ItemTypeObject)
{
#if !WITH_EDITOR
	return nullptr;
#else
	
	UEnum* EnumPtr = nullptr;

	switch (ItemTypeObject)
	{

		case EItemType::Weapon:
		{
			if (UEnum* WeaponEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWeaponType"), true))
			{
				EnumPtr = WeaponEnum;
			}
		}
		break;

		case EItemType::Helmet:
		case EItemType::Armor:
		case EItemType::Pant:
		case EItemType::Boots:
		{
			if (UEnum* ClothEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EClothType"), true))
			{
				EnumPtr = ClothEnum;
			}
		}
		break;
		case EItemType::Arrow:
		case EItemType::Enhancement:
		case EItemType::Expendable:
		{
			if (UEnum* ExpendableEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EExpendableType"), true))
			{
				EnumPtr = ExpendableEnum;
			}
		}
		break;
		case EItemType::Spell:
		{
			if (UEnum* SpellEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESpellType"), true))
			{
				EnumPtr = SpellEnum;
			}
		}
		break;

	default:
		break;
	}

#pragma warning(disable : 4996)
	return EnumPtr;

#endif
}

//찾지 못할경우 255를 띄움//
template<typename T>
uint8 GetMaxEnumValue(T Enum)
{
#if !WITH_EDITOR
	return 255;
#else
	static FString EnumName;
	static bool bEnumNameInit = false;

	// Init it once
	if (bEnumNameInit == false)
	{
		bEnumNameInit = true;

		FString FunctionSignature(__FUNCSIG__);
		UE_LOG(LogSoulItem, Display, TEXT("Enum funcsig: [%s]"), *FunctionSignature);

		int32 FirstIDX = FunctionSignature.Find(TEXT("(enum "), ESearchCase::CaseSensitive, ESearchDir::FromEnd);

		if (FirstIDX != INDEX_NONE)
		{
			FirstIDX = FirstIDX + 6;
			int32 LastIDX = (FunctionSignature.Len() - 1);

			if (FunctionSignature.IsValidIndex(FirstIDX) && FunctionSignature.IsValidIndex(LastIDX) && FirstIDX < LastIDX)
			{
				EnumName = FunctionSignature.Mid(FirstIDX, LastIDX - FirstIDX);
				UE_LOG(LogSoulItem, Display, TEXT("Enum name: [%s]"), *EnumName);
			}
		}
	}

	if (EnumName.IsEmpty())
		return 255;

	// const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, (const TCHAR *)(typeid(T).name()), true);
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!EnumPtr)
		return 255;

#pragma warning(disable: 4996)
	return EnumPtr->GetMaxEnumValue();
#endif
}
