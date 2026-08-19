// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIControllerBase.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
public:
	AAIControllerBase(FObjectInitializer const& Object_Initializer);
	virtual void BeginPlay() override;
	
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	UFUNCTION()
	void OnPerception(AActor* Actor, FAIStimulus Stimuls);
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	class UBlackboardComponent* GetBlackboard() const;

	void Setup_Perception_System();

	UFUNCTION()
	void UpdatedAttackTimer(float Min, float Max);
	UFUNCTION()
	void ClearAttackTimer();
	UFUNCTION()
	bool GetHasEnemysInSight();

private:
	void UpdateDetectedLevel();
	FRotator GetEnemyRotation(class ACharacterBase* Enemy);
	
	//UFUNCTION()
	void UpdatedCombatBehavior(float InDeltaTime);

	UFUNCTION()
	void CallAttackTimer();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	class UBehaviorTreeComponent* BTC;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	class UBlackboardComponent* BBC;
	class UAISenseConfig_Sight* Sight_Config;
	class UAISenseConfig_Hearing* Hearing_Config;

	UPROPERTY(BlueprintReadWrite)
	class AEnemyCharacter* Agent = nullptr;

	UPROPERTY(BlueprintReadWrite)
	class AActor* Manager;

	float StrafeDirection = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	float FollowDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	float CognizeLength = 700.f;

private:
	class ACharacterBase* Target;

	float DetectedLevel = 0.f;
	FTimerHandle StrafeTimer;
	FTimerHandle DetectedTimer;
	FTimerHandle AttackTimer;
};
