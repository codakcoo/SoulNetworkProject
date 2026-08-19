// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/EnemyCharacter.h"
#include "MonsterCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API AMonsterCharacter : public AEnemyCharacter
{
	GENERATED_BODY()
	
protected:
	AMonsterCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	UFUNCTION()
	virtual void SetupCharacter(bool bActivate) override;

protected:
	void EnemyDeathEvent() override;
};
