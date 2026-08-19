// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Classes/InventoryComponent.h"

#include "SoulNetworkProject/ItemData.h"
#include "SoulNetworkProject/CharacterState.h"

#include "Components/WidgetComponent.h"
#include "Components/TimeLineComponent.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdatedSoul, int32, InSoul, bool, bAnim);

USTRUCT(BlueprintType)
struct FInputType
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = Input)
	bool bButtonWhenAttack = false;
	UPROPERTY(BlueprintReadWrite, Category = Input)
	bool bButtonWhenDualAttack  = false;
	UPROPERTY(BlueprintReadWrite, Category = Input)
	bool bButtonWhenSpell = false;
	UPROPERTY(BlueprintReadWrite, Category = Input)
	bool bButtonWhenAOE = false;
	UPROPERTY(BlueprintReadWrite, Category = Input)
	bool bButtonWhenRoll = false;
	UPROPERTY(BlueprintReadWrite, Category = Input)
	bool bButtonWhenExpendable = false;

	//true�ϰ�� �ϳ��� true�̰� false�ϰܿ� ���ΰ��� false��¶��̴�.//
	bool InputCheckBoolean()
	{
		bool AllBoolean = false;
		if (bButtonWhenAttack) AllBoolean = true;
		else if (bButtonWhenDualAttack) AllBoolean = true;
		else if(bButtonWhenSpell) AllBoolean = true;
		else if (bButtonWhenAOE) AllBoolean = true;
		else if (bButtonWhenRoll) AllBoolean = true;
		else if (bButtonWhenExpendable) AllBoolean = true;

		return AllBoolean;
	}
	void AllTransFalse()
	{
		bButtonWhenAttack = false;
		bButtonWhenDualAttack = false;
		bButtonWhenSpell = false;
		bButtonWhenAOE = false;
		bButtonWhenRoll = false;
		bButtonWhenExpendable = false;
	}

	
};

USTRUCT(BlueprintType)
struct FAnimValues
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bCombat = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bCrouch = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bLowAttack = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bDualAttack = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bHeavyAttack = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bAirAttack = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bAOEAttack = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bSpellAttack = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bRoll = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bADS = false;
	
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bExpendable = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bOutofControl = false;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bAim = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bGuardUp = false;
	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool bMtMotion = false;

	//true�ϰ�� �ϳ��� true�̰� false�ϰܿ� ���ΰ��� false��¶��̴�.//
	/*
	*/
	bool ValueCheckBoolean()
	{
		bool AllBoolean = false;
		//if(bCrouch) AllBoolean = true;
		if(bLowAttack) AllBoolean = true;
		else if(bDualAttack) AllBoolean = true;
		else if(bHeavyAttack) AllBoolean = true;
		else if(bAOEAttack) AllBoolean = true;
		else if(bSpellAttack) AllBoolean = true;
		else if(bAirAttack) AllBoolean = true;
		else if(bRoll) AllBoolean = true;
		else if(bExpendable) AllBoolean = true;
		else if(bMtMotion) AllBoolean = true;
		else if(bOutofControl) AllBoolean = true;

		return AllBoolean;
	}
	bool IsAttacking()
	{
		bool bAttacking = false;

		if (bLowAttack) bAttacking = true;
		else if (bDualAttack) bAttacking = true;
		else if (bHeavyAttack) bAttacking = true;
		else if (bAOEAttack) bAttacking = true;
		else if (bSpellAttack) bSpellAttack = true;
		else if (bAirAttack) bAttacking = true;

		return bAirAttack;
	}
	void AllTransFalse()
	{
		//bCrouch = false; 
		bLowAttack = false;
		bDualAttack = false;
		bHeavyAttack = false;
		bAOEAttack = false;
		bSpellAttack = false;
		bAirAttack = false;
		bRoll = false;
		bExpendable = false;
		bOutofControl = false;
		bMtMotion = false;
		//bADS = false;
	}
	bool GetSlowValue()
	{
		if (bExpendable || bSpellAttack || bAim || bCrouch) return true;
		return false;
	}
};

UCLASS()
class SOULNETWORKPROJECT_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual  void PostInitializeComponents() override;
	
	virtual void Tick( float DeltaSeconds ) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintGetter)
	EFaction GetFaction() { return Faction; }

	UFUNCTION()
	void SetFaction(EFaction InFaction);// { Faction = InFaction; }

	UFUNCTION(Client, Reliable, WithValidation, BlueprintSetter)
	void Client_SetFaction(EFaction InFaction);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetFaction(EFaction InFaction);

	UFUNCTION(BlueprintSetter)
	void SetHasCrowdControl(bool bActive) { bHasCrowdControl = bActive; }
	UFUNCTION(BlueprintGetter)
	bool GetHasCrowdControl() { return bHasCrowdControl; }

	UFUNCTION()
	bool IsHostile(ACharacterBase* Chr);

	//true�ϰ�� ����, false�ϰ�� ǥ����//
	UFUNCTION()
	void SwitchHealthWidget(bool bActive) { HealthWidget->SetHiddenInGame(bActive); }

	UFUNCTION()
	float GetWalkSpeed() { return WalkSpeed; }
	UFUNCTION()
	float GetSprintSpeed() { return SprintSpeed; }
	UFUNCTION()
	float GetSlowSpeed() { return SlowWalkSpeed; }

	virtual void MeleeTrace(EEquipHand HandType) PURE_VIRTUAL(ACharacterBase, );

	virtual FRotator GetFocusRotator() const PURE_VIRTUAL(ACharacterBase, return FRotator::ZeroRotator; );

	//���� ����(Montage)��ȿ�� �˻��ϴ� �����Լ�//
	virtual void EndInputChecking() PURE_VIRTUAL(ACharacterBase, );
	//////////////////////////////////////////////


	virtual void EndMt() PURE_VIRTUAL(ACharacterBase, );

	//Player InputCallable//
	UFUNCTION(BlueprintCallable)
	void MoveForward(float Value);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);

	UFUNCTION(BlueprintCallable)
	void LookUp(float Value);
	UFUNCTION(BlueprintCallable)
	void Turn(float Value);

	//Jump//
	UFUNCTION(BlueprintCallable)
	void JumpStart();
	UFUNCTION(BlueprintCallable)
	void JumpEnd();
	///////
	//Crouch//
	UFUNCTION()
	void StartCrouch();
	UFUNCTION()
	void EndCrouch();

	UFUNCTION(Server, Reliable)
	void Server_ToggleCrouch(bool bActive);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ToggleCrouch(bool bActive);
	//////////

	UFUNCTION()
	void ToggleCombat(bool Newbool);
	UFUNCTION(Server, Reliable)
	void Server_ToggleCombat(bool Newbool);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ToggleCombat(bool Newbool);

	UFUNCTION()
	void ToggleADS(bool Newbool);
	UFUNCTION(Server, Reliable)
	void Server_ToggleADS(bool Newbool);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ToggleADS(bool Newbool);

	UFUNCTION(BlueprintCallable)
	void ToggleSpeed(float NewSpeed);
	UFUNCTION()
	void ToggleAim(bool Newbool);

	UFUNCTION(Server, Reliable)
	void Server_ToggleAim(bool Newbool);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ToggleAim(bool Newbool);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AttachItem(FInventoryData AttachItem);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DetachItem(EItemType ItemType, EEquipHand HandType, uint8 EquipIndex = 0);

	UFUNCTION()
	void ToggleGuard(bool Newbool);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ToggleGuard(bool Newbool);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_ToggleGuard(bool Newbool);

	//Basic LowAttack Montage//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_LowAttackMontage();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_LowAttackMontage();
	//////////////////

	//EquipType LowAttack Montage//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_E_LowAttackMontage();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_E_LowAttackMontage();
	/////////////////////

	//Basic HeavyAttack Montage//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_HeavyAttackMontage();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_HeavyAttackMontage();
	/////////////////////////////

	//EquipType HeavyAttack Montage//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_E_HeavyAttackMontage();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_E_HeavyAttackMontage();
	//////////////////////

	//Basic AOEAttack Montage//
	UFUNCTION(BlueprintCallable)
	void AOEAttackMontage();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AOEAttackMontage();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_AOEAttackMontage();
	///////////////////////////

	//EquipType AOEAttack Montage//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_E_AOEAttackMontage();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_E_AOEAttackMontage();
	///////////////////////////////

	//Basic DualAttack Montage//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DualAttackMontage();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_DualAttackMontage();

	//EquipType Spell Montage//
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_E_SpellMontage(FInventoryData InSpellData);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_E_SpellMontage(FInventoryData InSpellData);

	UFUNCTION(Server, Reliable)
	void Server_BowMontage(bool bPressedShift);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BowMontage(bool bPressedShift);

	UFUNCTION()
	void SpawnCastSpellName(class USkeletalMeshComponent* CastMesh, FName SpellName, FName SpawnSocket);
	UFUNCTION(Server, Reliable)
	void Server_SpawnCastSpellName(class USkeletalMeshComponent* CastMesh, FName SpellName, FName SpawnSocket);

	UFUNCTION()
	void SpawnCastSpell();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_SpawnCastSpell();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_LoopCastSpell();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_EndCastSpell();

	UFUNCTION(Server, Reliable)
	void Server_SpawnLowArrowActor();
	UFUNCTION(Server, Reliable)
	void Server_SpawnHeavyArrowActor();
	UFUNCTION(Server, Reliable)
	void Server_SpawnExpendableThrowActor();
	UFUNCTION()
	void SpawnThrowActor(FInventoryData ThrowData);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ClearSpellActor();
	///////////////////////////

	//Roll Montage//
	UFUNCTION(BlueprintCallable)
	void RollMontage(uint8 DirectionIndex);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RollMontage(uint8 DirectionIndex);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_RollMontage(uint8 DirectionIndex);
	////////////////

	//WeaponChange Montage// /* 0 has Left, 1 hase Right (Another Value Called is Blocked) */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_WeaponChangeMontage(float SwitchValue);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_WeaponChangeMontage(float SwitchValue);
	////////////////////////

	//Interaction Montage//
	UFUNCTION(Server, Reliable)
	void Server_InteractionMontage(class UAnimMontage* PlayMt, FName SectionName);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_InteractionMontage(class UAnimMontage* PlayMt, FName SectionName);
	///////////////////

	//�Է¿� ���� ���� �������ִ� �����Լ�//
	virtual void InputValueRollNDodge() PURE_VIRTUAL(ACharacterBase, );

	virtual void SetupCharacter(bool bActivate) PURE_VIRTUAL(ACharacterBase, );
	////////////////////////////////////////

	//����� ���� ȸ���ϴ� �Լ�//
	UFUNCTION(BlueprintCallable)
	void RunInterpInputVector(FRotator TargetRot);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RunInterpInputVector(FRotator TargetRot);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_RunInterpInputVector(FRotator TargetRot);
	/////////////////////////////

	UFUNCTION(BlueprintSetter)
	void SetUseOrientYaw(bool bActive) { bUseOrientYaw = bActive; }
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_SetUseOrientYaw(bool bActive);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_SetUseOrientYaw(bool bActive);

	UFUNCTION(BlueprintGetter)
	bool GetUseOrientYaw() { return bUseOrientYaw; }


	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdatedHandType(EEquipHand PermuteHandType);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_UpdatedHandType(EEquipHand PermuteHandType);

	//true�� ��¡�� ����. false�� ��¡�� �ȳ���.//
	UFUNCTION(BlueprintSetter)
	void SetCharging(bool bActive) { bCharging = bActive; }
	//true�� ��¡�� ����. false�� ��¡�� �ȳ���.//
	UFUNCTION(BlueprintGetter)
	bool GetCharging() { return bCharging; }

	UFUNCTION()
	bool IsBlockingAttack(AActor* DamageActor);
	UFUNCTION()
	bool IsPlayingMontage();

	UFUNCTION()
	FVector UpdatedLastLocation();
	UFUNCTION()
	void UpdatedStepEvent(FName SocketName, bool bUseEvent, float CueVolume = 1.0f);
	UFUNCTION()
	void UpdatedHitEvent(FVector HitLocation);
	UFUNCTION()
	void UpdatedLoudNoiseEvent(class USoundBase* CueAsset);

	
	UFUNCTION()
	void DropItem(FInventoryData& DropData, FVector SpawnLocation);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem(FInventoryData DropData, FVector SpawnLocation);
	/////////////////////////////////

	//UFUNCTION()
	//void RunDamagePossible(float Delay);

	//����//
	FORCEINLINE void SetCanInputCheck(bool bActive) { bCanInputCheck = bActive; }
	FORCEINLINE bool GetCanInputCheck() const { return bCanInputCheck; }

	//������ ������//
	FORCEINLINE FInventoryData GetMainWeapon() const { return MainWeapon; }
	FORCEINLINE FInventoryData GetSecondaryWeapon() const { return SecondaryWeapon; }
	FORCEINLINE FInventoryData GetIncreaseData() const { return IncreaseData; }
	FORCEINLINE FInventoryData GetSpellData() const { return SpellData; }

	FORCEINLINE class ACharacterBase* GetFocusActor() const { return FocusActor; }
	FORCEINLINE FName GetFocusBoneName() const { return FocusBoneName; }
	FORCEINLINE	class AMasterMagicActor* GetSpellActor() const { return CurrentSpellActor; }

	FInventoryData GetHoldTypeWeapon() const;
	UFUNCTION()
	EAIState GetStartAIState() const { return StartAIState; }
	//////////////

	//������Ʈ//
	UFUNCTION(BlueprintGetter)
	class UCharacterStateComponent* GetStateComponent() const { return StC; }
	UFUNCTION()
	class UCombatComponent* GetCombatComponent() const { return CbC; }
	UFUNCTION()
	class UEffectComponent* GetEffectComponent() const { return EfC; }
	////////////

	//�޽�//
	FORCEINLINE class USkeletalMeshComponent* GetMainWeaponMesh() const { return MainWeaponMesh; }
	FORCEINLINE class USkeletalMeshComponent* GetSecondaryWeaponMesh() const { return SecondaryWeaponMesh; }

	FORCEINLINE class USkeletalMeshComponent* GetMainShieldMesh() const { return MainShieldMesh; }
	FORCEINLINE class USkeletalMeshComponent* GetSecondaryShieldMesh() const { return SecondaryShieldMesh; }

	FORCEINLINE class USkeletalMeshComponent* GetBowWeaponMesh() const { return BowMesh; }
	////////

	//�Ͼư���//
	FORCEINLINE class UNiagaraComponent* GetNiagaraComponent() const { return Niagara; }
	////////////

	//��//
	FORCEINLINE EEquipHand GetHoldType() const { return HoldType; }

	FORCEINLINE int8 GetCurrentAttackCount() const { return CurrentLowAttackCount; }
	FORCEINLINE void SetLevel(int8 NextLevel) { Level = NextLevel; }
	FORCEINLINE int8 GetLevel() const { return Level; }
	FORCEINLINE FRotator GetConRot() const { return CurrentConRot; }
	FORCEINLINE class AAIControllerBase* GetControllerBase() const { return ControllerRef; }

	UFUNCTION()
	bool GetUseFadeMesh() const { return bUseFadeMesh; }

	UFUNCTION()
	bool GetDead() const { return bDead; }
	UFUNCTION()
	void SetDead(bool bActive) { bDead = bActive; }
	
	UFUNCTION()
	void SetSoul(uint64 InSoul) { Soul = InSoul;}
	UFUNCTION()
	uint64 GetSoul() const { return Soul; }
	UFUNCTION()
	void AddSoul(int32 InSoul);

	UFUNCTION()
	void SetUseHeavyArrow(bool bActivate) { bUseHeavyArrow = bActivate; }
	UFUNCTION()
	bool GetUseHeavyArrow() const { return bUseHeavyArrow; }
	//////

	UFUNCTION()
	void FadeInOut(bool bReverse);

	UFUNCTION()
	bool CanExecution();
	UFUNCTION(Server, Reliable)
	void CallExecution();

	UFUNCTION()
	void UpdatedWeaponMesh();

protected:
	UFUNCTION()
	FString GetSectionType(FInventoryData& UseData);

	//Test OnRepFunction//
	UFUNCTION()
	void OnRep_MainWeapon();
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	///////////////////////

	UFUNCTION()
	void UpdatedMainWeaponMesh(bool bHiddenMesh = false);
	UFUNCTION()
	void UpdatedSecondaryWeaponMesh(bool bHiddenMesh = false);

	//Player//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetFocusActor();
	virtual void SetFocusActor() PURE_VIRTUAL(ACharacterBase, );

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateConRotParam(FRotator InRotator);
	///////////////

	UFUNCTION()
	void TimelineFade(float Value);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UWidgetComponent* HealthWidget;


	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	FAnimValues AnimValues;
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	FInputType InputType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TreeAsset")
	class UBehaviorTree* TreeAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TreeAsset")
	class ASmartObject* SmartObject;


	UPROPERTY()
	uint8 CurrentLowAttackCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|MaxAttackCount")
	uint8 MaxLowAttackCount = 1;

	UPROPERTY()
	uint8 CurrentAOECount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|MaxAttackCount")
	uint8 MaxAOECount = 0;

	UPROPERTY()
	uint8 CurrentDualAttackCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|MaxAttackCount")
	uint8 MaxADualAttackCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeginData|AI")
	ECombatRole CombatRole;

	class AAIControllerBase* ControllerRef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamagedPossibleDelay = 0.5f;

	FTimerHandle DamagedHandle;
	//bool bDamagePossible = true;

	UPROPERTY()
	FUpdatedSoul UpdatedSoul;


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetFaction, Replicated, Category = "State")
	EFaction Faction = EFaction::Neutral;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	EAIState StartAIState = EAIState::Idle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SlowWalkSpeed = 94.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 240.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 375.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Montage")
	class UAnimMontage* LowAttackMt;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Montage")
	class UAnimMontage* HeavyAttackMt;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Montage")
	class UAnimMontage* AOEMt;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Montage")
	class UAnimMontage* DualAttackMt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll & Dodge|Montage")
	class UAnimMontage* RollMt;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* EquipAnimMontage;

	UPROPERTY(BlueprintSetter = SetCharging, BlueprintGetter = GetCharging)
	bool bCharging = false;
	UPROPERTY(BlueprintSetter = SetUseOrientYaw, BlueprintGetter = GetUseOrientYaw)
	bool bUseOrientYaw = false;

	UPROPERTY(BlueprintSetter = SetHasCrowdControl, BlueprintGetter = GetHasCrowdControl)
	bool bHasCrowdControl = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	bool bUseFadeMesh = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	EEquipHand HoldType = EEquipHand::None;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* OpacityTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* OpacityTimelineReverseCurve;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", BlueprintGetter = GetStateComponent)
	class UCharacterStateComponent* StC;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UCombatComponent* CbC;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UEffectComponent* EfC;

	//����//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* MainWeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* SecondaryWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* MainShieldMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* SecondaryShieldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* HoldWeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* HoldShieldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* BowMesh;
	////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	class UNiagaraComponent* Niagara;

	UPROPERTY(ReplicatedUsing = OnRep_MainWeapon)
	FInventoryData MainWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	FInventoryData SecondaryWeapon;

	UPROPERTY(Replicated)
	FInventoryData E_FirstArrow;
	UPROPERTY(Replicated)
	FInventoryData E_SecondArrow;



	class UHealthBarWidget* HealthBar;

	FInventoryData IncreaseData;
	FInventoryData SpellData;

	UPROPERTY(Replicated)
	class ACharacterBase* FocusActor = nullptr;
	UPROPERTY(Replicated)
	FName FocusBoneName = NAME_None;
	UPROPERTY(Replicated)
	class AMasterMagicActor* CurrentSpellActor = nullptr;


	UPROPERTY(Replicated)
	FRotator CurrentConRot;

	UPROPERTY(EditAnywhere, Category = "Soul")
	uint64 Soul = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	uint8 Level = 1;
	UPROPERTY(Replicated)
	bool bDead = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SubClass")
	TSubclassOf<class AThrowingKnifeActor> ThrowActorClass;
private:
	FTimerHandle ResetAttackTimer;
	FTimeline OpacityTimeline;

	UPROPERTY()
	bool bCanInputCheck = false;
	UPROPERTY()
	bool bUseHeavyArrow = false;
};
