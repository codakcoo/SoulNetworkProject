// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "Interface/InteractableInterface.h"
#include "RemainSoul.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ARemainSoul : public ASmartObject, public IInteractableInterface
{
	GENERATED_BODY()
	
protected:
	ARemainSoul();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
	/////////////

	UFUNCTION()
	void SetSoul(int64 InSoul) { Soul = InSoul < 0 ? 0 : InSoul; }
	UFUNCTION()
	int64 GetSoul() const { return Soul;}

protected:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	class UNiagaraSystem* AbsorptionNiagaraAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	class USoundBase* AbsorptionSoundAsset;

private:
	int64 Soul = 0;
};
 