// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "SoulNetworkProject/CharacterState.h"
#include "InteractionActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AInteractionActor : public ASmartObject
{
	GENERATED_BODY()
	
public:
	AInteractionActor();

	virtual void BeginPlay() override;


public:
	UFUNCTION(BlueprintSetter)
	void SetActiveActor(bool bActive) { bActiveActor = bActive; }
	UFUNCTION(BlueprintGetter)
	bool GetActiveActor() { return bActiveActor; }

	void OnInteration();
	void OffInteraction();

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		class USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UMenuWidget> PopupWidget_C;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Description")
	FName Description;

private:
	UPROPERTY(BlueprintSetter = SetActiveActor, BlueprintGetter = GetActiveActor)
	bool bActiveActor = false;

	EFaction Faction = EFaction::Neutral;

	class APlayerCharacter* Owner;
};
