// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "Interface/InteractableInterface.h"
#include "TutorialArea.generated.h"

/**
 * 
 */

UCLASS()
class SOULNETWORKPROJECT_API ATutorialArea : public ASmartObject, public IInteractableInterface
{
	GENERATED_BODY()

protected:
	ATutorialArea();

	virtual void BeginPlay() override;

	//Interface//
	UFUNCTION()
	virtual void Interact(class APlayerCharacter* Character) override;

	UFUNCTION()
	virtual FName GetInteractableName() const override;
	UFUNCTION()
	virtual void UpdatedOverlapEnd() override;
	UFUNCTION()
	virtual void PlayInteractionMt() override {}
	/////////////

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	void UpdateTriggerBox();



protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* TriggerArea;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	TArray<TSubclassOf<class UMenuWidget>> PopupWidgets_C;

private:
	class APlayerCharacter* CurrentOwner = nullptr;
	uint8 WidgetIndex = 0;
};
