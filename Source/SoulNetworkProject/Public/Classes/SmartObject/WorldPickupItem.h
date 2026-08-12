// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "Interface/InteractableInterface.h"
#include "SoulNetworkProject/ItemData.h"
#include "WorldPickupItem.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AWorldPickupItem : public ASmartObject, public IInteractableInterface
{
	GENERATED_BODY()
	

public:
	// Sets default values for this actor's properties
	AWorldPickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetupCollision();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		class USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		class UNiagaraComponent* Niagara;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataContents")
		TArray<FDataContent> DataContents;


private:
	//bool bPickup = false;

	FName Description;
};
