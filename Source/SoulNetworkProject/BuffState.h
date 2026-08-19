#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Class.h"
#include "Runtime/Core/Public/HAL/Platform.h"
#include "Containers/UnrealString.h"

UENUM(BlueprintType)
enum class EBuffRunType : uint8
{
	None		UMETA(DisplayName = "None"),
	Enchant		UMETA(DisplayName = "Enchant"),
	Enforce		UMETA(DisplayName = "Enforce")
};

UENUM(BlueprintType)
enum class EBuffIncreaseType : uint8
{
	Damage		UMETA(DisplayName = "Damage"),
	Defense		UMETA(DisplayName = "Defense")
};

UENUM(BlueprintType)
enum class EBuffEffectType : uint8
{
	Weapon		UMETA(DisplayName = "Weapon"),
	FullBody	UMETA(DisplayName = "Fullbody"),
	Center		UMETA(DisplayName = "Center")
};

UENUM(BlueprintType)
enum class EBuffElementType : uint8
{
	STR			UMETA(DisplayName = "STR"),
	INT			UMETA(DisplayName = "INT"),
	Fire		UMETA(DisplayName = "Fire"),
	Electric	UMETA(DisplayName = "Electric"),
	Ice			UMETA(DisplayName = "Ice")
};