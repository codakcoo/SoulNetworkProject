// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/CharacterBase.h"
#include "Perception/AISightTargetInterface.h"
#include "SoulNetworkProject/ItemData.h"
#include "Interface/CombatInterface.h"
#include "EnemyCharacter.generated.h"

/**
 * 
 */


UCLASS()
class SOULNETWORKPROJECT_API AEnemyCharacter : public ACharacterBase, public ICombatInterface, public IAISightTargetInterface
{
	GENERATED_BODY()
	
public:
	AEnemyCharacter();

	virtual void BeginPlay() override;

	virtual void Tick( float DeltaSeconds ) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed,
		float& OutSightStrength, const AActor* IgnoreActor = nullptr, const bool* bWasVisible = nullptr,
		int32* UserData = nullptr) const;

public:
	UFUNCTION()
	virtual void EndInputChecking() override;

	virtual void InputValueRollNDodge() override;

	//Interface//
	UFUNCTION()
	virtual void UpdatePointDamage(AActor* DamageCauser) override;
	UFUNCTION()
	virtual void UpdateRadialDamage(AActor* DamageCauser) override;
	UFUNCTION()
	virtual void UpdatedExecutionDamage(AActor* DamageCauser) override;
	UFUNCTION()
	virtual void UpdatedHitSurface(FHitResult HitResult, EAttackType AttackType) override;

	UFUNCTION()
	virtual void UpdatedSpell(float Value) override;
	UFUNCTION()
	virtual void UpdatedDeathFunc() override;

	UFUNCTION()
	virtual float GetApplyDamge(EEquipHand HandType) const override;
	/////////////

	FORCEINLINE TArray<FInventoryData> GetSpells() const { return SpellMap; }


	UFUNCTION()
	bool SetCurrentSpellCount(int8 PermuteValue);

	UFUNCTION()
	virtual void EnemyDeathEvent();

	UFUNCTION(Client, Reliable)
	void Server_SetSoult();

	UFUNCTION()
	virtual FRotator GetFocusRotator() const override;
	virtual void SetupCharacter(bool bActivate) override;

	UFUNCTION()
	void SetupObject(bool bActivate);
	UFUNCTION(Server, Reliable)
	void Server_SetupCharacter(bool bActivate);

	UFUNCTION()
	void RefreshCondition();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Refresh();

	UFUNCTION(Server, Reliable)
	void Server_RefreshWeaponMesh(bool bHiddenMesh);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RefreshWeaponMesh(bool bHiddenMesh);

	UFUNCTION()
	void SetLookRotation(FVector EventLocation);

	UFUNCTION(Server, Reliable)
	void Server_EndMt();

protected:
	UFUNCTION()
	virtual void SetFocusActor() override;
	UFUNCTION()
	virtual void EndMt() override;
	UFUNCTION()
	void TestFunction();

	UFUNCTION()
	void SetupDataContent();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StackSpellData(FInventoryData StackData);

	virtual void MeleeTrace(EEquipHand HandType) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BeginData|Content")
	TArray<FDataContent> DataContents;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BeginData|Content")
	TArray<FDataContent> DropItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FName PerceptionTarget = "spine_02";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MaxLength = 100000.0f;




	UPROPERTY()
	class AAIControllerBase* Con;
	UPROPERTY(Replicated)
	TArray<class ACharacterBase*> HostileChrs;

private:
	UPROPERTY(Replicated)
	TArray<FInventoryData> SpellMap;
	
	uint8 CurrentSpellCount = 0;

	FVector StartLoc = FVector::ZeroVector;
	FRotator StartRot = FRotator::ZeroRotator;

	FRotator LookRotation  = FRotator::ZeroRotator;
};
