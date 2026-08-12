// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockonSystemComponent.generated.h"

USTRUCT(BlueprintType)
struct FLockData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ACharacterBase* LockChr = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName LockBoneName = NAME_None;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULNETWORKPROJECT_API ULockonSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULockonSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	void CheckDirectionScreenPoint(float AxisValue, bool bForward);
	UFUNCTION()
	void CheckDirectionYaw(float AxisValue, bool bForward);


	UFUNCTION()
	class ACharacterBase* GetLockChr() const { return LockData.LockChr; }
	UFUNCTION()
	FName GetLockBoneName() const { return LockData.LockBoneName; }

	UFUNCTION()
	void StartLock();
	UFUNCTION()
	void SetFocusActor(AActor* InActor, FName BoneName = NAME_None);
	UFUNCTION(Server, Reliable)
	void Server_SetFocusActor(AActor* InActor, FName BoneName = NAME_None);
	UFUNCTION(Client, Reliable)
	void BindWidget(AActor* BindActor, FName BindBoneName, bool bVisibility);
	UFUNCTION(Client, Reliable)
	void Client_UpdatedConRot(FRotator InRot);

private:
	UFUNCTION()
	void UpdatedLockon(float InDeltaTime);
	UFUNCTION(Server, Reliable)
	void Server_UpdatedLockon(float InDeltaTime);

	UFUNCTION(Server, Reliable)
	void UpdatedSphereDebug(float InDeltaTime);
	UFUNCTION()
	TArray<FLockData> GetSearchAgents();
	UFUNCTION()
	FLockData GetFindOptimalTarget();

	UFUNCTION()
	float CalculateDotProductToTarget(class ACharacterBase* Target, FName TargetSocketName);
	UFUNCTION()
	float CalcualteDotProductToTargetY(class ACharacterBase* Target, FName TargetSocketName);
	UFUNCTION()
	float GetLocalYawToTargetFromLockChr(class ACharacterBase* Target, FName TargetSocketName);
	UFUNCTION()
	float GetLocalYawToTargetFromCO(class ACharacterBase* Target, FName TagetSocketName);
	UFUNCTION()
	float GetDepthDistanceToTaget(class ACharacterBase* Target, FName TargetSocketName);

	UFUNCTION()
	float GetScreenPositionOfActor(AActor* SearchActor, FName SearchBoneName);

	/* Requires Co, LockChr, Target these valid to work. */
	UFUNCTION()
	bool CanSearchTargetInArea(class ACharacterBase* Target, FName TargetSocketName);

	TTuple<float, bool> GetScreenXDistanceOfActor(AActor* SearchActor, FName SearchBoneName);	
	TTuple<float, bool> GetScreenYDistanceOfActor(AActor* SearchActor, FName SearchBoneName);
	float GetScreenSizeRatio();
	bool HasActorInViewport(AActor* SearchActor, FName SearchBoneName);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	float InterpSpeed = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	float TimerDelay = 0.5f;

	// Min: 0.0f and Max: 90.0f If the Over value appply 0.0f in value.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	float DetectAngle = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	float MaxSphereRadius = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	float MaxSearchDistanceLockChrToTarget = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	float MaxSearchDistanceCoToTarget = 3000.0f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	bool bShowDebug = false;

private:
	UPROPERTY(Replicated)
	FLockData LockData;
	UPROPERTY(Replicated)
	bool bCanSwitch = true;

	UPROPERTY(VisibleAnywhere)
	float MaxBlindTime = 5.f;
	UPROPERTY(Replicated)
	float CurrentBlineTime = 0.f;

	FTimerHandle SwitchHandle;

	float InputY = 0.f;
	float InputX = 0.f;
};
