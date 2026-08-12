// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ManagerInterface.h"
#include "SoulNetworkProject/CharacterState.h"
#include "AIManager.generated.h"

UCLASS()
class SOULNETWORKPROJECT_API AAIManager : public AActor, public IManagerInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	bool Engaged();

	UFUNCTION(BlueprintCallable)
	void NotifyAIState(EAIState State);

	UFUNCTION()
	TArray<class ACharacterBase*> GetAgents() const { return Agents; }
	UFUNCTION()
	bool GetCanAttack();
	UFUNCTION()
	bool GetCanFollow();

	UFUNCTION()
	void SetupAgents(TArray<bool> Activates);
	UFUNCTION()
	void RefreshAgents();
	UFUNCTION()
	void AllTransFalseforOnlyDeathValueInAgents();

	// Interface Function //
	UFUNCTION()
	virtual bool UpdatedAttackInManager(class ACharacterBase* ApplyPawn) override;
	///////////////////////

private:
	void CreateControlList();
	void SetTargetAllAgent();

	void RunCombatLoop();
	void RunSearchLoop();
	void RunMandownLoop();

	bool GetAnyoneFollow();

public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<class ACharacterBase*> Agents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float CheckDistance = 1000.f;

	UPROPERTY()
	FVector LastStimulusLocation;

protected:
	TArray<class AAIControllerBase*> Controls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EFaction Faction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	uint8 MaxAttackCount = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	uint8 MaxFollowCount = 3;

	FTimer SearchTimer;
	FTimer CombatTimer;
	FTimerHandle MandownTimer;

};
