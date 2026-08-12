// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Magic/MasterMagicActor.h"
#include "MultipleCastSpellActor.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class ESpawnType : uint8
 {
	AtOnce		UMETA(DisplayName = "AtOnce"),
	Duration	UMETA(DisplayName = "Duration")
 };
 UENUM(BlueprintType)
 enum class ESpawnRandomType : uint8
 {
	None				UMETA(DisplayName = "None"),
	LocationOnly		UMETA(DisplayName = 'LocationOnly'),
	RotationOnly		UMETA(DisplayName = "RotationOnly"),
	Both				UMETA(DisplayName = "Both")
 };

UENUM(BlueprintType)
enum class EMultipleType : uint8
{
	High		UMETA(DisplayName = "High"),
	Forward		UMETA(DisplayName = "Forward"),
	Ground		UMETA(DisplayName = "Ground")
};

UCLASS()
class SOULNETWORKPROJECT_API AMultipleCastSpellActor : public AMasterMagicActor
{
	GENERATED_BODY()
	
public:
	AMultipleCastSpellActor();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	virtual void BeginData(class ACharacterBase* InOwner, class USkeletalMeshComponent* CastMesh) override;


	UFUNCTION()
	virtual void LoopData() override;


	virtual void EndData() override;

private:
	virtual void UpdatedCollisionEvent(float InDeltaTime) override;

	void SpawnSubSpell();

	TTuple<FVector, FRotator> GetSpawnTransform();

	FVector GetLocalLocation();
	FVector GetRandeomLocation();
	FRotator GetLocalRotation();
	FRotator GetRandomRotation();

	void EndScale();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TSubclassOf<AMasterMagicActor> ProjectileSpellClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	ESpawnType SpawnType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	ESpawnRandomType SpellRandomType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EMultipleType MultipleType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FVector AttachedVector = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FRotator AttachedRotaor = FRotator::ZeroRotator;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	uint8 SpawnCount;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FVector RangeVector = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MinRangeLook = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxRangeLook = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MinRangeTurn = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxRangeTurn = 0.0f;

	// Not work Roll. //
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FRotator RangeRotator = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (EditCondition = "SpawnRandomType == SpellRandomType == ESpawnRandomType::RotationOnly || SpellRandomType == ESpawnRandomType::Both"))
	float MinRangePItch = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (EditCondition = "SpawnRandomType == SpellRandomType == ESpawnRandomType::RotationOnly || SpellRandomType == ESpawnRandomType::Both"))
	float MaxRangePItch = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (EditCondition = "SpawnRandomType == SpellRandomType == ESpawnRandomType::RotationOnly || SpellRandomType == ESpawnRandomType::Both"))
	float MinRangeYaw = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (EditCondition = "SpawnRandomType == SpellRandomType == ESpawnRandomType::RotationOnly || SpellRandomType == ESpawnRandomType::Both"))
	float MaxRangeYaw = 0.0f;

	class ACharacterBase* TrackChr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float LoopTime;
	FTimerHandle LoopTimer;
};
