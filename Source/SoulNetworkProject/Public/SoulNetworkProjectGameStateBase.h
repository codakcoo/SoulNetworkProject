// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interface/GameStateInterface.h"
#include "SoulNetworkProjectGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ASoulNetworkProjectGameStateBase : public AGameStateBase, public IGameStateInterface
{
	GENERATED_BODY()
	
protected:
	ASoulNetworkProjectGameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

public:

	// Interface //
	UFUNCTION()
	virtual void SetServerTimer(bool bActivate) override;
	UFUNCTION()
	virtual void CheckNumofPlayer(int8 NumofPlayer) override;
	UFUNCTION()
	virtual void DestroyServerReaction() override;

	UFUNCTION()
	virtual void SetCurrentBM(class ABossManager* EnterManager) override;
	UFUNCTION()
	virtual class ABossManager* GetCurrentBM() override;
	UFUNCTION()
	virtual void UpdatedAreaActorsAtBM() override;
	UFUNCTION()
	virtual void StartBossArea(class APlayerCharacter* EnterPawn) override;
	UFUNCTION()
	virtual void SetBossWidget(bool bVisible, APawn* ApplyPawn) override;
	UFUNCTION()
	virtual void CallEvnetUI(TArray<class ACharacterBase*> EventPawns, EEventUIType EventType) override;


	UFUNCTION()
	virtual void StartBossSequenceEvent() override;
	UFUNCTION()
	virtual void EndBossSequenceEvent() override;
	UFUNCTION()
	virtual void SkipCinematic() override;

	UFUNCTION()
	virtual void LoadWorldData() override;
	UFUNCTION()
	virtual void RefreshWorldData() override;
	////////////////

	UFUNCTION()
	void SetBossArea(bool bActivate);
	UFUNCTION(Server, Reliable)
	void Server_SetBossArea(bool bActivate);

	UFUNCTION(Server, Reliable)
	void Server_CloseServer();

	UFUNCTION(Server, Reliable)
	void Server_LoadWorldData();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_LoadWorldData();

private:
	UFUNCTION()
	void KickLostPlayerInBossArea();
	UFUNCTION(Server, Reliable)
	void Server_SkipCinematic();

protected:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float OpenServerTimer = 180.0f;
	float OriginServerTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float OpenStartBossTimer = 30.0f;
	float OriginStartBossTimer;

	UPROPERTY()
	bool bOpernServer = false;
	UPROPERTY()
	bool bStartBossArea = false;

	UPROPERTY()
	class ABossManager* CurrentBM;
};
