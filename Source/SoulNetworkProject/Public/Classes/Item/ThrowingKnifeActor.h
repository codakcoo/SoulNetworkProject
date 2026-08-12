// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "SoulNetworkProject/ItemData.h"
#include "SoulNetworkProject/CharacterState.h"
#include "ThrowingKnifeActor.generated.h"

UCLASS()
class SOULNETWORKPROJECT_API AThrowingKnifeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowingKnifeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick( float DeltaSeconds ) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	UFUNCTION()
	void SetupData(FInventoryData Data, class ACharacterBase* SetupOwner);

	UFUNCTION(Server, Reliable)
	void Server_SetupMesh(class USkeletalMesh* Mesh);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetupMesh(class USkeletalMesh* Mesh);

	FORCEINLINE class USkeletalMeshComponent* GetMesh() { return KnifeMesh; }

private:
	UFUNCTION()
	void UpdatedTrace();

protected:
	FTimerHandle LifeTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	float LifeTime = 15.f;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* KnifeMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UProjectileComponent* Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* ElementNiagaraComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* TrailNigaraComp;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageType")
	TSubclassOf<class UDamageType> ThrowDamageType_C;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	class UNiagaraSystem* FireNiagara;

	UPROPERTY(Replicated)
	EFaction Faction;

	UPROPERTY(Replicated)
	class ACharacterBase* ApplyOwner;
	UPROPERTY(Replicated)
	float Damage = 0.f;
};
