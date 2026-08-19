// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulNetworkProject/ItemState.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathEventUpdated);

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None	UMETA(DisplayName = "None"),
	Low		UMETA(DisplayName = "Low"),
	Heavy	UMETA(DisplayName = "Heavy"),
	Upper	UMETA(DisplayName = "Upper"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULNETWORKPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//���� Ʈ���̽�//
	UFUNCTION()
	void WeaponTrace(EEquipHand HandType, EAttackType AttackType, FName StartSocket = NAME_None, FName EndSocket = NAME_None);
	UFUNCTION(Server, Reliable)
	void Server_WeaponTrace(EEquipHand HandType, EAttackType AttackType, FName StartSocket = NAME_None, FName EndSocket = NAME_None);
	/////////////////

	UFUNCTION()
	bool GetGuardup() { return bGuardup; }


	//ó��//
	UFUNCTION(Server, Reliable)
	void Server_Attacker(class ACharacterBase* VictimPawn, FName PlaySection);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Attacker(class ACharacterBase* VictimPawn, FName PlaySection);

	UFUNCTION(Server, Reliable)
	void Server_Victim(class ACharacterBase* AttackerPawn, FName PlaySection);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Victim(class ACharacterBase* AttackerPawn, FName PlaySection);

	UFUNCTION()
	void ExecutionDamage();
	UFUNCTION(Server, Reliable)
	void Server_ExecutionDamage();
	////////

	//����//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RunStun();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_RunStun();

	//���� ����Ʈ//

	////////////////

	UFUNCTION()
	TSubclassOf<class UDamageType> GetDamageType(EAttackType AttackType);
	UFUNCTION()
	TSubclassOf<class UDamageType> GetExecutionDamageType() { return ExecutionImpact_C; }
	
	//FORCEINLINE void SetAttackType(EAttackType InType);
	//Get// //Set//
	void SetParryingParam(float DelayTime);

	FORCEINLINE bool GetParrying() const { return bParrying; }

	FORCEINLINE void SetFalseHitSurface() { bHitSurface = false; }

	FORCEINLINE void SetOutofControl(bool bActive) { bOutofControl = bActive; }
	FORCEINLINE bool GetOutofControl() { return bOutofControl; }

	FORCEINLINE bool GetHitSurface() const { return bHitSurface; }

	UFUNCTION()
	bool GetHasExecutionAttackerMt() const { return StrikeAttacker_Mt ? true : false;}
	UFUNCTION()
	bool GetHasExecutionVictimMt() const { return StrikeVictim_Mt ? true : false; }

	UFUNCTION()
	bool GetUseForwardTrigger() const { return bUseForwardTrigger; }
	UFUNCTION()
	bool GetUseBehindTrigger() const { return bUseBehindTrigger; }
	UFUNCTION()
	bool GetAlwaysUseForwardTrigger() const { return bAlwaysUseForwardTrigger; }
	UFUNCTION()
	bool GetAlwaysUseBehindTrigger() const { return bAlwaysUseBehindTrigger; }

	UFUNCTION()
	class ACharacterBase* GetCurrentAttacker() const { return CurrentAttacker; }

	UFUNCTION()
	class ACharacterBase* GetCurrentVictim() const { return CurrentVictim; }

	UFUNCTION()
	void SetFallingHurt(bool bActive);
	UFUNCTION()
	bool GetFallingHurt() { return bFallingHurt; }
	UFUNCTION()
	bool IsStrongHurt();
	UFUNCTION()
	bool IsFallingWithStrongHurt();
	//////


	UFUNCTION()
	void GetForwardVector(FVector InLocation);
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_GetForwardVector(FVector InLocation);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_RunDeadMotion();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_RunDeadMotion();

	//Strong HurtMotion//
	void RunHurtMotion();
	//Low HurtMotion//
	void RunHurtMotion(FRotator InCauserRot);
	//Heavy HurtMotion//
	void RunHurtMotion(FVector OriginLoc);
	//Guard HurtMotion//
	void RunGuardMotion(EAttackType InType);

	UFUNCTION()
	void CallDeathEvent(float Delay);
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_CallDeathEvent(float Delay);

	UFUNCTION()
	void Ragdoll();
	UFUNCTION(Server, Reliable)
	void Server_Ragdoll();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Ragdoll();

	UFUNCTION()
	void UpdatedCameraShakeArea(FVector AreaLocation, float AreaRadius, EAttackType AttackType);
	UFUNCTION()
	void RunCameraShakeAtChr(class ACharacterBase* InChr, EAttackType AttackType);
	UFUNCTION()
	void RunCameraShakeAtActors(TArray<AActor*> InActors, EAttackType AttackType);


	UFUNCTION()
	float GetCameraShakeRadiusType(EAttackType AttackType);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetupCollision(bool bActive);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_SetupCollision(bool bActive);

	UFUNCTION()
	void SphereDamageArea(FVector AreaLocation, float AreaRadius, EAttackType AttackType);
	UFUNCTION(Server, Reliable)
	void Server_SphereDamageArea(FVector AreaLocation, float AreaRadius, EAttackType AttackType);

	UFUNCTION()
	bool CanExecution(class ACharacterBase* InPawn, FName TriggerTag);

	UFUNCTION()
	void UpdatedBlockCollision(bool bActivate);

private:

	//Guard up//
	UFUNCTION()
	void UpdatedShieldTrace(float DeltaTime);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdatedShieldTrace(float DeltaTime);
	///////////
	UFUNCTION()
	void UpdatedRagdoll(float DeltaTime);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdatedRagdoll(float DeltaTime);
	/////////

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_HurtMotion(class UAnimMontage* HurtMt, FName SectionName = NAME_None);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_HurtMotion(class UAnimMontage* HurtMt, FName SectionName = NAME_None);
	UFUNCTION(Server, Reliable)
	void Server_SetFallingHurt(bool bActive);

	//0 = Start, 1 = End//
	TTuple<FVector, FVector> GetWeaponLocs(class USkeletalMeshComponent* TraceMesh, FName StartSocketName, FName EndSocketName);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLocation(FVector NewLocation);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetRotation(FRotator NewRotation);



public:
	UPROPERTY()
	FOnDeathEventUpdated OnDeathEventUpdated;


protected:
	UPROPERTY(Replicated)
	bool bGuardup = false;


	//WeaponTrace//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info|Trace")
	bool bShowedDraw = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info|Trace")
	float BasicTraceSize = 20.f;
	///////////////

	//ó��//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution")
	bool bAlwaysUseForwardTrigger = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution")
	bool bAlwaysUseBehindTrigger = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution")
	bool bUseForwardTrigger = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution")
	bool bUseBehindTrigger = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution")
	class UAnimMontage* StrikeAttacker_Mt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Execution")
	class UAnimMontage* StrikeVictim_Mt;
	///////

	//OutofControle//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stun")
	class UAnimMontage* Stun_Mt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dead")
	class UAnimMontage* Dead_Mt;
	////////////////

	//Hurt//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hurt")
	class UAnimMontage* LowHurt_Mt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hurt")
	class UAnimMontage* StrongHurt_Mt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hurt")
	class UAnimMontage* GuardHurt_Mt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hurt")
	class UAnimMontage* FallingStrongHurt_Mt;
	////////

	//Damage Type//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageType")
	TSubclassOf<class UDamageType> LowImpact_C;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageType")
	TSubclassOf<class UDamageType> HeavyImpact_C;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageType")
	TSubclassOf<class UDamageType> UpperImpact_C;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageType")
	TSubclassOf<class UDamageType> ExecutionImpact_C;
	//////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageType")
	float LowCameraShakeRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageType")
	float HeavyCameraShakeRadius = 200.0f;


	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
	//class UNiagaraSystem* SmokeNigara_Obj;

	UPROPERTY(Replicated)
	bool bFallingHurt = false;

private:
	//EAttackType AttackType = EAttackType::None;

	FTimerHandle ParryingTimer;
	FTimerHandle DeathEventTimer;


	bool bParrying = false;
	bool bHitSurface = false;
	bool bRagdoll = false;
	bool bOutofControl = false;

	class ACharacterBase* CurrentAttacker;
	class ACharacterBase* CurrentVictim;
};
