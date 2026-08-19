// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/CharacterBase.h"
#include "Classes/InventoryComponent.h"
#include "Classes/LockonSystemComponent.h"
#include "Classes/ClothComponent.h"
#include "Interface/CombatInterface.h"

#include "SoulNetworkProject/PlayerHUD.h"
#include "SoulNetworkProject/ItemData.h"


#include "PlayerCharacter.generated.h"

/**
 * 
 */


UENUM(BlueprintType)
enum class EInputKey : uint8
{
	None	UMETA(DisplayName = "None"),
	LMB		UMETA(DisplayName = "LMB"),
	RMB		UMETA(DisplayName = "RMB"),
	SPB		UMETA(DisplayName = "SPB"),
	RKB		UMETA(DisplayName = "RKB")
};

UCLASS()
class SOULNETWORKPROJECT_API APlayerCharacter : public ACharacterBase, public ICombatInterface
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//UFUNCTION()
	//void OnRep_ChangeBodyColor();

	FString GetEnumToText(ENetRole NetRole);

public:
	UFUNCTION()
	void LowAttack();
	UFUNCTION()
	void HeavyAttack();
	UFUNCTION()
	void AOEAttack();
	UFUNCTION()
	void WandAttack();
	UFUNCTION()
	void TalismanAttack();


	UFUNCTION(BlueprintCallable)
	virtual void EndInputChecking() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ToggleSpeed(float InSpeed);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_ToggleSpeed(float InSpeed);


	UFUNCTION(BlueprintCallable)
	void SetInputValue(float AxisValue, bool bForward);


	UFUNCTION()
	void AttachEquipItem(FInventoryData AttachData, FName SocketName = NAME_None);
	UFUNCTION()
	void RemoveEquipItem(EItemType ItemType, EEquipHand HandType = EEquipHand::None);

	FORCEINLINE class UCameraComponent* GetCamera() { return Camera; }
	FORCEINLINE class USpringArmComponent* GetSpringArm() { return SpringArm; }
	FORCEINLINE class UCameraComponent* GetTopViewCamera() { return TopCamera; }


	UFUNCTION(BlueprintSetter)
	void SetStartingStand(bool bActive) { AnimValues.bMtMotion = bActive; }
	UFUNCTION(BlueprintGetter)
	bool GetStartingStand() { return AnimValues.bMtMotion; }

	UFUNCTION(BlueprintSetter)
	void SetCharacterMoving(bool bActive) { bCharacterMoving = bActive; }
	UFUNCTION(BlueprintGetter)
	bool GetCharacterMoving();

	UFUNCTION(BlueprintCallable)
	void StartMotion();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_StartMotion();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_StartMotion();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AirAttack();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_AirAttack();

	UFUNCTION()
	virtual void MeleeTrace(EEquipHand HandType) override;


	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_UseExpendable(FInventoryData UseExpendableData);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_UseExpendable(FInventoryData UseExpendableData);


	UFUNCTION(Client, Reliable)
	void Client_BindBossActor(class ABossCharacter* BindActor);

	FORCEINLINE class USkeletalMeshComponent* GetHelmetMesh() { return Eq_Helmet; }
	FORCEINLINE class USkeletalMeshComponent* GetArmorMesh() { return Eq_Armor; }
	FORCEINLINE class USkeletalMeshComponent* GetPantMesh() { return Eq_Pant; }
	FORCEINLINE class USkeletalMeshComponent* GetBootsMesh() { return Eq_Boots; }

	FORCEINLINE UInventoryComponent* GetInventoryComponent() { return Inventory; }
	FORCEINLINE ULockonSystemComponent* GetLockonComponent() { return Lockon; }
	FORCEINLINE UClothComponent* GetClothComponent() { return Cloth; }

	FORCEINLINE FInventoryData GetExpendableData() { return E_Expendable; }
	FORCEINLINE FInventoryData Get_E_SpellData() { return E_Spell; }

	FORCEINLINE FInventoryData Get_E_Helmet() const { return E_Helmet; }
	FORCEINLINE FInventoryData Get_E_Armor() const { return E_Armor; }
	FORCEINLINE FInventoryData Get_E_Pant() const { return E_Pant; }
	FORCEINLINE FInventoryData Get_E_Boots() const { return E_Boots; }

	UFUNCTION()
	FInventoryData Get_E_ClothData(EItemType ItemType) const;

	FORCEINLINE class APlayerHUD* GetHUD() const { return PlayerHUD; }
	FORCEINLINE class ABossCharacter* GetTargetBoss() { return TargetBoss; }

	FORCEINLINE float GetInputX() const { return InputX; }
	FORCEINLINE float GetInputY() const { return InputY; }

	FORCEINLINE int8 GetInteractionIndex() const { return InteractionIndex; }
	FORCEINLINE void SetInteractionIndex(int8 IncreaseValue) { InteractionIndex = IncreaseValue; }

	FORCEINLINE void SetPowerup(bool bActive) { bPowerup = bActive; }
	FORCEINLINE void SetInteract(bool bActive) { bInteract = bActive; }

	FORCEINLINE bool GetPowerup() const { return bPowerup; }
	FORCEINLINE bool GetInteract() const { return bInteract; }

	FORCEINLINE class APlayerControllerBase* GetPC() const { return ControlBase; }

	FORCEINLINE bool GetActiveInteraction() const { return bActiveInteraction; }

	UFUNCTION()
	FText GetInteractionText(AActor* InteractionActor);

	UFUNCTION()
	void UpdateInputKey(EInputKey InputKey);

	UFUNCTION()
	virtual void EndMt() override;

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_CallEventUI(EEventUIType EventType);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CallMessageUI(FName OutputName);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Client_CallMessageUI(FName OutputName);

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

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_PointEvent();

	UFUNCTION(BlueprintCallable)
	virtual void InputValueRollNDodge() override;
	UFUNCTION()
	bool IsInteractionActor(AActor* InteractionActor);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_OverlapActors(bool bVisible, const FText& InText = FText());
	UFUNCTION(Client, Reliable, WithValidation)
		void Client_OverlapActors(bool bVisible, const FText& InText = FText());
	
	UFUNCTION()
	virtual FRotator GetFocusRotator() const override;

	UFUNCTION()
	void BindProperty(class APlayerControllerBase* Con, class APlayerHUD* HUD);



	UFUNCTION()
	void ClearPopupWidget();

	UFUNCTION(Server, Reliable)
	void Server_RunInteraction(class AActor* SmartObj, FVector NewLoc, FRotator NewRot);

	UFUNCTION(Client, Reliable)
	void Client_PopupMenu(AActor* ParentActor, TSubclassOf<class UUserWidget> PopupClass, bool bGamePaused);

	UFUNCTION(Client, Reliable)
	void Client_OverlapInteraction(AActor* OverlapActor);
	UFUNCTION(Client, Reliable)
	void Client_EndOverlapInteraction(AActor* OverlapActor);

	UFUNCTION(Server, Reliable)
	void Server_RefreshManagers();

	UFUNCTION(Server, Reliable)
	void Server_SetUseControllerYaw(bool bActive);

	UFUNCTION()
	bool IsMainSpell();
	UFUNCTION()
	bool IsSecondarySpell();

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void UpdatedEquipItem(EItemType SlotType, EEquipHand HandType, FInventoryData ItemData, bool bAnim);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PermuteData(EItemType ApplyItemType, FInventoryData PermuteData);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ResetData(EItemType ItemType);

	UFUNCTION()
	void SetupBodyComp();
	UFUNCTION()
	void SetBodyAsset();

	//OnRep Function//
	UFUNCTION()
	void OnRep_Helmet();
	UFUNCTION()
	void OnRep_Armor();
	UFUNCTION()
	void OnRep_Pant();
	UFUNCTION()
	void OnRep_Boots();
	UFUNCTION()
	void OnRep_Expendable();
	/////////////////

	//Focus Actor//
	UFUNCTION()
	virtual void SetFocusActor() override;

	UFUNCTION()
	void PlayerDeathEvent();
	
	UFUNCTION(Server, Reliable)
	void Server_SetRotation(FRotator NewRot);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetRotation(FRotator NewRot);
	
	UFUNCTION(Server, Reliable)
	void Server_SetLocation(FVector NewLoc);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLocation(FVector NewLoc);

	UFUNCTION()
	void UpdatedLastInputVector();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector InputVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Lockon")
	ULockonSystemComponent* Lockon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Inventory")
	UInventoryComponent* Inventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Cloth")
	UClothComponent* Cloth;	
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* StartStandAnimMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info")
	FName PlayerName;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* InteractionMt;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* AirAttackMt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* EnterAreaMt;

	//Equipment Test//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Eq_Helmet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Eq_Armor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Eq_Pant;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Eq_Boots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Body_Chest;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Body_Arms;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Body_hands;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Body_Legs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Body_Feets;

	UPROPERTY(ReplicatedUsing = OnRep_Helmet)
		FInventoryData E_Helmet;
	UPROPERTY(ReplicatedUsing = OnRep_Armor)
		FInventoryData E_Armor;
	UPROPERTY(ReplicatedUsing = OnRep_Pant)
		FInventoryData E_Pant;
	UPROPERTY(ReplicatedUsing = OnRep_Boots)
		FInventoryData E_Boots;

	UPROPERTY(ReplicatedUsing = OnRep_Expendable)
		FInventoryData E_Expendable;
	UPROPERTY(Replicated)
		FInventoryData E_Spell;

	/////////////////
	bool bUseAirAttack = false;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* TopViewSpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* L_Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* R_Collision;

	UPROPERTY()
	float InputX;
	UPROPERTY()
	float InputY;
	UPROPERTY()
	FRotator InputRot;
	UPROPERTY()
	FVector InputConVector;
	UPROPERTY()
	FVector LastInputConVector;
	
	
	UPROPERTY(BlueprintSetter = SetStartingStand, BlueprintGetter = GetStartingStand)
	bool bStartingStand = false;
	UPROPERTY(BlueprintSetter = SetCharacterMoving, BlueprintGetter = GetCharacterMoving)
	bool bCharacterMoving = false;

	UPROPERTY()
	bool bPowerup = false;
	UPROPERTY()
	bool bInteract = false;

	//Interaction (��ȣ�ۿ�)//
	bool bActiveInteraction = false;
	int8 InteractionIndex = 0;

	class ABossCharacter* TargetBoss = nullptr;
	//UPROPERTY(Replicated)
	class APlayerHUD* PlayerHUD;
	//UPROPERTY(Replicated)
	class APlayerControllerBase* ControlBase = nullptr;
};
