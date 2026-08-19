// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "Interface/InteractableInterface.h"
#include "Interface/EventInterface.h"
#include "CheckPoint.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ACheckPoint : public ASmartObject, public IInteractableInterface, public IEventInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACheckPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:

	//Interface//
	UFUNCTION()
	virtual void Interact(class APlayerCharacter* Character) override;

	UFUNCTION()
	virtual FName GetInteractableName() const override;
	UFUNCTION()
	virtual void UpdatedOverlapEnd() override;
	UFUNCTION()
	virtual void PlayInteractionMt() override;

	UFUNCTION()
	virtual void CallEvent() override;
	/////////////

	UFUNCTION()
	virtual void SetupObject(bool bCondition) override;

private:
	UFUNCTION()
	void UpdatedScaleParam(float InDeltaTime);
	UFUNCTION()
	float GetFloatNiagaraParam(FName VariableName);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UNiagaraComponent* Niagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	class UAnimMontage* CheckInteractionMt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	class UAnimMontage* RestInteractionMt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	class UNiagaraSystem* CheckNSAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapData")
	FName PointName;

private:
	//bool bPickup = false;
	FName Description;
	bool bPlayScale = false;
	bool bActivate = true;
};
