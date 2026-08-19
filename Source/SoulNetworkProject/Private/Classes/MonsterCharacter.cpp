// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/MonsterCharacter.h"
#include "Classes/CombatComponent.h"
#include "Classes/EffectComponent.h"

#include "Components/SphereComponent.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMonsterCharacter::SetupCharacter(bool bActivate)
{
	if (!GetEffectComponent()) return;

	if (bActivate)
	{
		GetEffectComponent()->AllMeshFadeout(1.0f);
		SetDead(false);
	}
	else
	{
		GetEffectComponent()->AllMeshFadeout(0.0f);
		SetDead(true);
	}
}

void AMonsterCharacter::EnemyDeathEvent()
{
	FadeInOut(true);
}
