// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/EnemyCharacter.h"
#include "BossCharacter.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class EPhaseType : uint8
 {
	None		UMETA(DisplayName = "None"),
	Enchant		UMETA(DisplayName = "Enchant"),
	Health		UMETA(DisplayName = "Health"),
 };


 USTRUCT(BlueprintType)
 struct FBossPhaseState
 {
	GENERATED_USTRUCT_BODY()
 public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPhaseType PhaseType = EPhaseType::Enchant;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "PhaseType == EPhaseType::Health"))
	uint8 Numerator = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "PhaseType == EPhaseType::Health"))
	uint8 Denominator = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 PhaseIndex = 0;
 };

UCLASS()
class SOULNETWORKPROJECT_API ABossCharacter : public AEnemyCharacter
{
	GENERATED_BODY()
	
protected:
	ABossCharacter();

	virtual void BeginPlay() override;

	virtual void Tick( float DeltaSeconds ) override;

public:
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SkillMontage(uint8 SkillIndex);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_SkillMontage(uint8 SkillIndex);

	UFUNCTION()
	void StartBossStage(ACharacterBase* EnterActor);
	UFUNCTION()
	void SetManager(class ABossManager* OwnerManager);

	UFUNCTION()
	bool HasActorInArea(AActor* CheckActor);
	FORCEINLINE bool GetCanUseSkillTree(uint8 UseIndex) { return SkillArr.Num() < UseIndex ? false : true; }
	UFUNCTION(BlueprintGetter)
	class ABossManager* GetBossManager() const { return BossManager; }
	UFUNCTION()
	virtual void SetupCharacter(bool bActivate) override;

	UFUNCTION()
	int8 GetBossPhaseIndex() const;


protected:
	virtual void EnemyDeathEvent() override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Boss Montage")
	TArray<class UAnimMontage*> SkillArr;

protected:
	UPROPERTY(BlueprintGetter = GetBossManager)
	class ABossManager* BossManager = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	FBossPhaseState BossPhaseState;
};
