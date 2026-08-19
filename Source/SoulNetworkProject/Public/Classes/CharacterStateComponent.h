// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "SoulNetworkProject/BuffState.h"
#include "CharacterStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatedHealth, bool, bAnim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatedMana, bool, bAnim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatedStamina, bool, bAnim);


UENUM(BlueprintType)
enum class EOperatorType : uint8
{
	Stop	UMETA(DisplayName = "Stop"),
	Add		UMETA(DisplayName = "Add"),
	Min		UMETA(DisplayName = "Min")
};

USTRUCT(BlueprintType)
struct FChrStat : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		float Health = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		float Mana = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		float Stamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float STR = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float INT = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float DEF = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float MR = 5.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float PoiseDamage = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float PoiseHealth = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	uint8 NextExp = 20.0f;

	void SetData(FChrStat PermuteData)
	{
		Health = PermuteData.Health;
		Mana = PermuteData.Mana;
		Stamina = PermuteData.Stamina;
		STR = PermuteData.STR;
		INT = PermuteData.INT;
		DEF = PermuteData.DEF;
		MR = PermuteData.MR;
		PoiseDamage = PermuteData.PoiseDamage;
		PoiseHealth = PermuteData.PoiseHealth;
		NextExp = PermuteData.NextExp;
	}
};

USTRUCT(BlueprintType)
struct FBuffData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FName DataName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float BuffIncreaseValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EBuffRunType RunType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EBuffElementType ElementType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EBuffIncreaseType IncreaseType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EBuffEffectType EffectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	FTimerHandle BuffTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float TimerDelay = 5.f;

	bool IsSet()
	{
		return RunType != EBuffRunType::None && BuffIncreaseValue > 0.f ;
	}
	void Reset()
	{
		DataName = NAME_None;
		BuffIncreaseValue = 0.f;

		RunType = EBuffRunType::None;
		ElementType = EBuffElementType::STR;
		IncreaseType = EBuffIncreaseType::Defense;
		EffectType = EBuffEffectType::FullBody;

		TimerDelay = 5.f;
	}
	bool operator == (const FBuffData& rhs) const 
	{
		if(this->DataName == rhs.DataName
			&& this->BuffIncreaseValue == rhs.BuffIncreaseValue 
			&& this->RunType == rhs.RunType
			&& this->ElementType == rhs.ElementType
			&& this->IncreaseType == rhs.IncreaseType
			&& this->EffectType == rhs.EffectType
			&& this->TimerDelay == rhs.TimerDelay)
				return true;

		return false;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULNETWORKPROJECT_API UCharacterStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	void AddHealth(float InHealth);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddHealth(float InHealth);

	UFUNCTION()
	void AddMana(float InMana);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddMana(float InMana);

	UFUNCTION(BlueprintCallable)
	void AddStamina(float InStamina);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddStamina(float InStamina);

	UFUNCTION()
	void AddCurrentPoiseHealth(float InPoiseHealth);

	UFUNCTION()
	void OnDamagePossible(float Delay);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnDamagePossible(float Delay);

	UFUNCTION()
	void SetDamagePossible(bool bActive);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetDamagePossible(bool bActive);

	UFUNCTION(Server, Reliable)
	void SetCurrentHealth(float InValue);// { CurrentStat.Health = InValue; }
	UFUNCTION(Server, Reliable)
	void SetCurrentMana(float InValue);// { CurrentStat.Mana = InValue; }

	FORCEINLINE float GetMaxHealth() const { return MaxStat.Health; }
	FORCEINLINE float GetMaxMana() const { return MaxStat.Mana; }
	FORCEINLINE float GetMaxStamina() const { return MaxStat.Stamina; }

	FORCEINLINE float GetSTR() const { return CurrentStat.STR; }
	FORCEINLINE float GetINT() const { return CurrentStat.INT; }

	FORCEINLINE float GetDEF() const { return CurrentStat.DEF; }
	FORCEINLINE float GetMR() const { return CurrentStat.MR; }

	FORCEINLINE float GetMaxPoiseDamage() const { return MaxStat.PoiseDamage; }
	FORCEINLINE float GetMaxPoiseHealth() const { return MaxStat.PoiseHealth; }

	FORCEINLINE float GetCurrentHealth() const { return CurrentStat.Health; }
	FORCEINLINE float GetCurrentMana() const { return CurrentStat.Mana; }
	FORCEINLINE float GetCurrentStamina() const { return CurrentStat.Stamina; }

	FORCEINLINE float GetCurrentPoiseDamage() const { return CurrentStat.PoiseDamage; }
	FORCEINLINE float GetCurrentPoiseHealth() const { return CurrentStat.PoiseHealth; }
	
	FORCEINLINE bool GetUseHyperArmor() const { return bUseHyperArmor; }
	FORCEINLINE void SetUseHyperArmor(bool bActive) { bUseHyperArmor = bActive; }

	FORCEINLINE bool GetUseRigidity() const { return bUseRigidity; }
	FORCEINLINE bool GetDamagePossible() const { return bDamagePossible; }

	FORCEINLINE FBuffData GetEnchant() const { return EnchantBuff; }
	FORCEINLINE TArray<FBuffData> GetEnforces() const { return EnforceBuffs; }

	UFUNCTION()
	uint8 GetNextExp(uint8 NextLevel);
	UFUNCTION()
	int32 GetDecreaseHealth() const { return DecreaseHealth; }

	UFUNCTION()
	void ResetPoiseStat();

	UFUNCTION()
	void SetOperator(EOperatorType InOperatorType);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetOperator(EOperatorType InOperatorType);

	UFUNCTION(BlueprintCallable)
	void RunBuffData(FName RowName);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RowBuffData(FName RowName);

	UFUNCTION()
	void OnRep_Enchant();
	UFUNCTION()
	void OnRep_Enforce();
	
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_ClearEnchant();

	UFUNCTION()
	void SetupState();

	UFUNCTION()
	void UpdatedIncreaseData();

private:
	UFUNCTION()
	void UpdateStamina(float InDeltaTime);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateStamina(float InDeltaTime);
	
	FChrStat* GetChrStatRow(int8 RowLevel);
	FBuffData* GetBuffDataRow(FName RowName);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetupState();

	UFUNCTION(Server, Reliable)
	void Server_BindProperty();


	UFUNCTION(Client, Reliable)
	void Client_PercentBroadcast(FName PermuteName, bool bAnim);
	UFUNCTION(Server, Reliable)
	void Server_UpdatedIncreaseData();


public:
	UPROPERTY(Replicated)
	EOperatorType OperatorType = EOperatorType::Stop;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float StaminaDelay = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float StaminaIncrease = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* StatDt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* BuffDt;

	FUpdatedHealth OnHealthUpdated;
	FUpdatedMana OnManaUpdated;
	FUpdatedStamina OnStaminaUpdated;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", Replicated)
	bool bUseRigidity = true;


private:
	FTimerHandle StaminaHandle;
	FTimerHandle DamagePossibleHandle;

	UPROPERTY(Replicated)
	class ACharacterBase* Owner;

	UPROPERTY(Replicated)
	FChrStat MaxStat;
	UPROPERTY(Replicated)
	FChrStat CurrentStat;

	UPROPERTY(Replicated)
	int32 DecreaseHealth = 0;
	FTimerHandle DecreaseHandle;
	float DecreaseDelay = 2.0f;

	UPROPERTY(Replicated)
	bool bUseHyperArmor = false;
	UPROPERTY(Replicated)
	bool bDamagePossible = true;

	UPROPERTY(ReplicatedUsing = OnRep_Enchant)
	FBuffData EnchantBuff;
	UPROPERTY(ReplicatedUsing = OnRep_Enforce)
	TArray<FBuffData> EnforceBuffs;

};
