// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "Interface/InteractableInterface.h"
#include "Components/TimeLineComponent.h"
#include "BossEntrance.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ABossEntrance : public ASmartObject, public IInteractableInterface
{
	GENERATED_BODY()
	
protected:
	ABossEntrance();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

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
	void SetEntrance(bool bActivate);
	UFUNCTION()
	void SetEntranceWithFade(bool bActivate);
	UFUNCTION()
	void RefreshObject();

protected:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void TimelineFade(float Value);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Component")
	class UStaticMeshComponent* WallMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Component")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Montage")
	class UAnimMontage* EnterAreaMt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A|Timeline")
	UCurveFloat* OpacityTimelineCurve;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A|Timeline")
	UCurveFloat* OpacityTimelineReverseCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Data", meta = (MakeEditWidget = "true"))
		FVector LastSpawnLocation;

private:
	UPROPERTY()
	FTimeline OpacityTimeline;

private:
	bool bDoorActivate = false;

	FName Description = NAME_None;
};
