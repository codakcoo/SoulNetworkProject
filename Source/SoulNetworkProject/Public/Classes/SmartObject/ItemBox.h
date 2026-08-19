// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "Interface/InteractableInterface.h"
#include "SoulNetworkProject/ItemData.h"
#include "Components/TimelineComponent.h"
#include "ItemBox.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AItemBox : public ASmartObject, public IInteractableInterface
{
	GENERATED_BODY()
	
protected:
	AItemBox();

	virtual void BeginPlay() override;

	virtual void Tick( float DeltaSeconds ) override;

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

protected:
	UFUNCTION()
	void OpenTopValue(float value);
	UFUNCTION()
	void FinishedOpen();

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetupCollision();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UStaticMeshComponent* TopMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
		class UBoxComponent* Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		class UAudioComponent* Audio;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		class USoundBase* ButtonCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	TArray<FDataContent> DataContents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	class UAnimMontage* InteractionMt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	class UCurveFloat* MovingfCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	FVector TargetLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	FRotator TargetRot;

private:
	FTimeline TopMeshTimeline;

	FOnTimelineFloat MoveFloat;
	FOnTimelineEvent MoveEnd;

	FVector StartLoc = FVector(-29.000000, 0.000000, 27.000000);
	FRotator StartRot = FRotator(0.0f);
};
