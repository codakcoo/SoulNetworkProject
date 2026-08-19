// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Magic/MasterMagicActor.h"
#include "StrikeSpellActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AStrikeSpellActor : public AMasterMagicActor
{
	GENERATED_BODY()

	/*
	*/
public:
	AStrikeSpellActor();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UFUNCTION()
		virtual void BeginData(class ACharacterBase* InOwner, class USkeletalMeshComponent* CastMesh) override;
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetupNiagaraValue();

	UFUNCTION()
		virtual void LoopData() override;


	UFUNCTION()
		virtual void MultipleData(class ACharacterBase* InOwner);

	UFUNCTION()
		virtual void EndData() override;

private:
	virtual void UpdatedCollisionEvent(float InDeltaTime) override;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_UpdatedCollisionEvent(float InDeltaTime);

	UFUNCTION()
	void CallDestroy() { Destroy(); }
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Strike", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ARangeSpellActor> RangeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Strike", meta = (AllowPrivateAccess = "true"))
	float RangeEnd = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Strike", meta = (AllowPrivateAccess = "true"))
	float ColorScale = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Strike", meta = (AllowPrivateAccess = "true"))
	float InterpSpeed = 10.f;

private:
	//UPROPERTY(Replicated)
	//bool bEndScale = false;

	UPROPERTY(Replicated)
	TArray<class ACharacterBase*> HitActors;
	UPROPERTY(Replicated)
	bool bHitSurface = false;
};
