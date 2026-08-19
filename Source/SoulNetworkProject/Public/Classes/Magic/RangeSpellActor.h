// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Magic/MasterMagicActor.h"
#include "RangeSpellActor.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class ERangeType : uint8
 {
	Self		UMETA(DisplayName = "Self"),
	Around		UMETA(DisplayName = "Around"),
	Track		UMETA(DisplayName = "Track")
 };

UCLASS()
class SOULNETWORKPROJECT_API ARangeSpellActor : public AMasterMagicActor
{
	GENERATED_BODY()
	
public:
	ARangeSpellActor();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	virtual void BeginData(class ACharacterBase* InOwner, class USkeletalMeshComponent* CastMesh) override;

	UFUNCTION()
	virtual void LoopData() override;
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_SetupNiagaraValue();

	UFUNCTION()
	virtual void MultipleData(class ACharacterBase* InOwner);

	UFUNCTION()
	virtual void EndData() override;

private:
	virtual void UpdatedCollisionEvent(float InDeltaTime) override;

	//UFUNCTION(NetMulticast, Reliable, WithValidation)
	//void Multicast_Reaction(FTransform SpawnTransform);

	void LastReaction(class ACharacterBase* InOwner);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_LastReaction();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
	ERangeType RangeType = ERangeType::Self;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Around", meta = (EditCondition = "RangeType == ERangeType::Around"))
	FVector RangeVector = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Around", meta = (EditCondition = "RangeType == ERangeType::Around"))
	bool bGroudnDetach = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Range", meta = (EditCondition = "RangeType == ERangeType::Track"))
	uint8 TrackCount = 6;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Range", meta = (EditCondition = "RangeType == ERangeType::Track"))
	float EndReactionRadius = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Range", meta = (EditCondition = "RangeType == ERangeType::Track"))
	float TrackLoopTime = 0.1;

private:
	UPROPERTY(Replicated)
	class ACharacterBase* TrackChr = nullptr;
	UPROPERTY(Replicated)
	FName TrackBoneName = NAME_None;
	UPROPERTY(Replicated)
	ARangeSpellActor* LastTrackActor = nullptr;

	FTimerHandle TrackLoopTimer;
	UPROPERTY(Replicated)
	uint8 CurrentCount;
};
