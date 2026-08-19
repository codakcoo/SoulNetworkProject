// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameframework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "SoulNetworkProject/ItemData.h"
#include "PickupActor.generated.h"

UCLASS()
class SOULNETWORKPROJECT_API APickupActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:

	//Interface//
	UFUNCTION()
	virtual void Interact(class APlayerCharacter* Character) override;

	UFUNCTION()
	virtual FName GetInteractableName() const override;
	UFUNCTION()
	virtual void UpdatedOverlapEnd() override {}
	UFUNCTION()
	virtual void PlayInteractionMt() override;
	/////////////

	UFUNCTION()
	TArray<FDataContent> GetDataContents() const { return DataContents; }

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetupContent(FInventoryData Data);

	UFUNCTION()
	void ContainContents(TArray<FDataContent> Contents) { DataContents = Contents; }

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	void SetupContent(FDataContent& Data);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UNiagaraComponent* Niagara;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataContents", Replicated)
	TArray<FDataContent> DataContents;


private:
	bool bPickup = false;

	FName Description;
};
