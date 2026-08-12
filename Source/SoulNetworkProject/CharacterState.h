#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Class.h"
#include "Runtime/Core/Public/HAL/Platform.h"


UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle   = 0           UMETA(DisplayName = "Idle"),
	ManDown   =1        UMETA(DisplayName = "Man Down"),
	Investigate = 2      UMETA(DisplayName = "Investigate"),
	LostEnemy   = 3      UMETA(DisplayName = "Lost Enemy"),
	Search       =4     UMETA(DisplayName = "Search"),
	Attack        =5    UMETA(DisplayName = "Attack")
};

UENUM(BlueprintType)
enum class EFaction : uint8
{
	Enemy            UMETA(DisplayName = "Enemy"),
	Friendly         UMETA(DisplayName = "Friendly"),
	Neutral          UMETA(DisplayName = "Neutral")
};

UENUM(BlueprintType)
enum class ECombatRole : uint8
{
	Melee           UMETA(DisplayName = "Melee"),
	Range			UMETA(DisplayName = "Range"),
	All				UMETA(DisplayName = "All")
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Follow			UMETA(DisplayName = "Follow"),
	Strafe			UMETA(DisplayName = "Strafe"),
	Attack          UMETA(DisplayName = "Attack"),
	Stun			UMETA(DisplayName = "Stun")
	//HoldCover         UMETA(DisplayName = "HoldCover"),
	//RetreatingCover   UMETA(DisplayName = "RetreatingCover"),
	//Evade             UMETA(DisplayName = "Evade")
};
