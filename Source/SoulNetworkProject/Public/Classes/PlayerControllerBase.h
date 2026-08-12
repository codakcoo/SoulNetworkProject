// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EHoldKeyState : uint8
{
	None	UMETA(DisplayName = "None"),
	Ctrl	UMETA(DisplayName = "Ctrl"),
	Space	UMETA(DisplayName = "Space")
};

UENUM(BlueprintType)
enum class EShakeType : uint8
{
	LowShake		UMETA(DisplayName = "LowShake"),
	HeavyShake		UMETA(DisplayName = "HeavyShake")
};


UCLASS()
class SOULNETWORKPROJECT_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
protected:
	APlayerControllerBase(FObjectInitializer const& Object_Initializer);
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void SetupInputComponent() override;

public:
	UFUNCTION()
	void LowShakeCamera();

	UFUNCTION(Server, Reliable)
	void PlayFromEndDeathEvent();

	UFUNCTION()
	class APlayerCharacter* GetPossessPawn() const { return PossessPawn; }
	UFUNCTION()
	FName GetPlayerName() const { return PlayerName; }
	UFUNCTION()
	FVector GetLastLocation() const { return LastLocation; }

	UFUNCTION(Server, Reliable)
	void Server_SendJoinMessage();
	UFUNCTION(Server, Reliable)
	void Server_RefreshMeshs();

	UFUNCTION(Client, Reliable)
	void BroadcastMessage(FName DyingMessage);
	UFUNCTION(Client, Reliable)
	void BroadcastCloth();

	UFUNCTION(BlueprintCallable)
	void TopView();

	UFUNCTION(Client, Reliable)
	void OneRateCS(EShakeType ShakeType);

	UFUNCTION()
	void ChangeActiveMenu();

	UFUNCTION()
	void AllClearTimer();

	UFUNCTION()
	void CallCheckPointMenu();

	//UFUNCTION(Client, Reliable)
	//void LoadWorld();
	UFUNCTION(Client, Reliable)
	void Client_SavePlayerData();
	UFUNCTION(Client, Reliable)
	void Client_SaveLostSoul();
	UFUNCTION(Client, Reliable)
	void Client_ClearLostSoul();

	UFUNCTION(Client, Reliable)
	void Client_SaveCurrentCondition();
	UFUNCTION(Client, Reliable)
	void Client_SaveMaxCondition();
	UFUNCTION(Client, Reliable)
	void Client_SetCurrentCondition();

	UFUNCTION(Client, Reliable)
	void Client_CallEventUI(EEventUIType EventType);
	UFUNCTION(Client, Reliable)
	void Client_FadeScreen(bool bFadeIn);
	UFUNCTION(Client, Reliable)
	void Client_SetBossWidget(bool bVisible);

	UFUNCTION(Client, Reliable)
	void SetCinematic(bool bCinematic);
	UFUNCTION(Client, Reliable)
	void SetInput(bool bInput);

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void Lookup(float AxisValue);
	void Turn(float AxisValue);

	void LMBClicked();
	void LMBReleased();

	void RMBClicked();
	void RMBReleased();

	void JumpStart();
	void JumpEnd();

	void SprintPressed();
	void SprintReleased();

	void CrouchPressed();
	void CrouchReleased();

	void QKBPressed();
	void EKBPressed();
	void EKBReleased();
	void RKBPressed();

	void SfBPressed();
	void SfBReleased();

	void SwitchLeft();
	void SwitchRight();
	void SwitchUp();
	void SwitchDown();

	void SwitchAction(float AxisValue);
	void ESCPressed();

	void LeftMenu();
	void RightMenu();

	void UpdateHoldKey(float InDeltaTime);

	UFUNCTION(BlueprintCallable)
	void ToggleKeyState(EHoldKeyState ChangeState);

	UFUNCTION(Server, Reliable)
	void Server_ToggleKeyState(EHoldKeyState ChangeState);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ToggleKeyState(EHoldKeyState ChangeState);

	UFUNCTION(Server, Reliable)
	void Server_KilledGM(class APlayerControllerBase* Con);

	UFUNCTION(Server, Reliable)
	void Server_Lockon();
	UFUNCTION(Server, Reliable)
	void Server_AxisLock(float AxisValue, bool bVertical);

	UFUNCTION(Server, Reliable)
	void Server_UseExpendable();

private:
	bool CanUseInput();

protected:
	UPROPERTY(VisibleAnywhere)
	float CurrentSlowTime = 0.f;
	UPROPERTY(VisibleAnywhere)
	float CurrentSprintTime = 0.f;
	UPROPERTY(VisibleAnywhere)
	float MaxHoldTime = 0.2f;

	UPROPERTY(VisibleAnywhere)
	float UpdateLocationDelay = 5.0f;

	UPROPERTY(Replicated)
	class APlayerCharacter* PossessPawn;
	UPROPERTY(Replicated)
	bool bPlayingExecution;
private:
	class APlayerCameraManagerBase* PCM;
	EHoldKeyState KeyState = EHoldKeyState::None;

	UPROPERTY(Replicated)
	FName PlayerName = TEXT("Player1");

	UPROPERTY()
	FVector LastLocation = FVector::ZeroVector;
	FTimerHandle LastLocationHandle;

	class APlayerHUD* HUD;
};
