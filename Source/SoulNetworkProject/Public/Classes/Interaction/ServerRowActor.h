// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/InteractionActor.h"
#include "SignPointActor.h"
#include "Interface/InteractableInterface.h"
#include "ServerRowActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AServerRowActor : public AInteractionActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AServerRowActor();

public:
	UFUNCTION()
	void SetServerNameToString(FServerData ServerData);

	UFUNCTION()
	void Setup(uint8 InIndex);

	//Interface//
	UFUNCTION()
	virtual void Interact(class APlayerCharacter* Character) override;

	UFUNCTION()
	virtual FName GetInteractableName() const override;

	UFUNCTION()
	virtual void UpdatedOverlapEnd() override;

	UFUNCTION()
	virtual void PlayInteractionMt() override;
	/////////////

	FORCEINLINE FText GetServerName() const { return ServerName; }
	FORCEINLINE uint8 GetNumofCurrentPlayer() const { return NumofCurrentPlayer; }
	FORCEINLINE uint8 GetNumofMaxPlayer() const { return NumofMaxPlayer; }
	FORCEINLINE TOptional<uint8> GetServerIndex() const { return ServerIndex; }

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Server")
	FText ServerName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Server")
	uint8 NumofCurrentPlayer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Server")
	uint8 NumofMaxPlayer;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* InteractionMt;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TOptional<uint8> ServerIndex;

private:
	class UServerRowWidget* CurrentWidget = nullptr;

	class APlayerCharacter* CurrentOwner = nullptr;
};
