// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "BossTrigger.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ABossTrigger : public ASmartObject
{
	GENERATED_BODY()
	
protected:
	ABossTrigger();


public:
	UFUNCTION()
	void SetManager(class ABossManager* InManager) { BossManager = InManager; }
	UFUNCTION()
	void SetTrigger(bool bActivate);

protected:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UBoxComponent* Trigger;

private:
	class ABossManager* BossManager;

};
