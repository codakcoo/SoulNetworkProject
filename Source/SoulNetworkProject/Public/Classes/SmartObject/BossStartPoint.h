// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "BossStartPoint.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API ABossStartPoint : public ASmartObject
{
	GENERATED_BODY()
	
protected:
	ABossStartPoint();

	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void StartSequence();

protected:
	UPROPERTY(VisibleAnywhere)
	class ALevelSequenceActor* LevelSequenceActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Sequence")
	class ULevelSequence* LevelSequence;
	UPROPERTY(VisibleAnywhere)
	class ULevelSequencePlayer* LevelSequencePlayer;

};
