// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interface/CharacterInterface.h"
#include "SoulNetworkProjectPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ASoulNetworkProjectPlayerState : public APlayerState, public ICharacterInterface
{
	GENERATED_BODY()
	
protected:
	ASoulNetworkProjectPlayerState(const FObjectInitializer& ObjectInitializer);

	virtual void ClientInitialize(class AController* C) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

public:
	UFUNCTION()
	void SetDead(bool bActive) { bDead = bActive; }
	UFUNCTION()
	bool GetDead() const { return bDead; }

	UFUNCTION()
	void SetPlayerIndex(int8 InIndex) { PlayerIndex = InIndex; }
	UFUNCTION()
	int8 GetPlayerIndex() const { return PlayerIndex; }

	UFUNCTION(Server, Reliable)
	void SetControllerBase(class APlayerControllerBase* Con);
	UFUNCTION(Server, Reliable)
	void SetPotionCount(uint8 HealthCount, uint8 ManaCount);



	// Interface //
	UFUNCTION()
	virtual void SetupState(class APlayerCharacter* ApplyPawn) override;
	UFUNCTION()
	virtual void SetInventory(class APlayerCharacter* ApplyPawn) override;
	UFUNCTION()
	virtual void SetDeathEvent(class APlayerCharacter* ApplyPawn) override;

	UFUNCTION()
	virtual void SetVisibilityTimer(bool bActivate) override;
	UFUNCTION()
	virtual void UpdatedServerTimer(float InTimer) override;
	UFUNCTION()
	virtual void KickedPlayer() override;
	UFUNCTION()
	virtual void UpdatedWarpPoint() override;
	UFUNCTION()
	virtual void UpdatedCheckPoint() override;
	UFUNCTION()
	virtual void UpdatedRespawnPoint() override;

	UFUNCTION()
	virtual void SetupStartCharacter(class APlayerCharacter* ApplyPawn) override;
	UFUNCTION()
	virtual void SetupStartPoint(class APlayerCharacter* ApplyPawn) override;


	UFUNCTION()
	virtual void SetEnterBossArea(bool bActive) override;
	UFUNCTION(Server, Reliable)
	void Server_SetEnterBossArea(bool bActive);
	UFUNCTION()
	virtual bool GetEnterBossArea() override;
	UFUNCTION()
	virtual void StartBossAreaBroadcast() override;


	UFUNCTION()
	virtual void JoinEvnet() override;
	UFUNCTION()
	virtual void DeathEvent() override;
	UFUNCTION()
	virtual void ExitEvent() override;
	/////////////////

	UFUNCTION(Client, Reliable)
	void Server_SetupState(class APlayerCharacter* ApplyPawn);
	UFUNCTION(Client, Reliable)
	void Server_SetInventory(class APlayerCharacter* ApplyPawn);

	UFUNCTION(Client, Reliable)
		void Client_StartCharacter(class APlayerCharacter* ApplyPawn);
	UFUNCTION(Client, Reliable)
		void Client_StartPoint(class APlayerCharacter* ApplyPawn);

private:
	UFUNCTION(Client, Reliable)
	void Client_JoinEvent();
	UFUNCTION(Client, Reliable)
	void Client_DeathEvent();
	UFUNCTION(Client, Reliable)
	void Client_ExitEvent();

	UFUNCTION(Client, Reliable)
	void Client_SetVisibilityTimer(bool bActivate);
	UFUNCTION(Client, Reliable)
	void Client_UpdatedTimer(float InTimer);

	UFUNCTION(Server, Reliable)
	void Server_SpawnStartPointFromAuthGM(APlayerCharacter* ApplyPawn);
	//UFUNCTION(Server, Reliable)
	//void Server_SpawnCheckPointFromAuthGM(APlayerCharacter* ApplyPawn);

	UFUNCTION(Client, Reliable)
	void KickedoftheServer();

	UFUNCTION(Client, Reliable)
	void Client_StartBossArea();

	UFUNCTION(Server, Reliable)
	void Server_CallGMSendMessage(FName Message);

	UFUNCTION(Server, Reliable)
	void Server_SetAlphaCharacterMesh(class APlayerCharacter* ApplyPawn, float Alpha);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetAlphaCharacterMesh(class APlayerCharacter* ApplyPawn, float Alpha);

	UFUNCTION(Server, Reliable)
	void Server_FadeCharacterMesh(bool bFadeIn);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FadeCharacterMesh(bool bFadeIn);

	UFUNCTION(Server, Reliable)
	void Server_StartSequence(class APlayerCharacter* ApplyPawn);
	UFUNCTION()
	void StartSequence(class APlayerCharacter* ApplyPawn);

protected:
	UPROPERTY(Replicated)
	int8 PlayerIndex = 0;
	UPROPERTY(Replicated)
	bool bDead = false;
	UPROPERTY(Replicated)
	bool bEnterBossArea = false;

	UPROPERTY(Replicated)
	uint8 MaxHealthPotionCount = 0;
	UPROPERTY(Replicated)
	uint8 MaxManaPotionCount = 0;

	UPROPERTY(Replicated)
	class APlayerControllerBase* ControllerBase = nullptr;

private:
	FTimerHandle FadeTimer;		
};
