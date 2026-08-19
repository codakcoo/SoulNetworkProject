// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/BossTrigger.h"
#include "Classes/BossManager.h"
#include "Classes/PlayerCharacter.h"

#include "Components/BoxComponent.h"
#include "SoulNetworkDebug.h"

ABossTrigger::ABossTrigger()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(GetRootComponent());
	Trigger->SetCollisionProfileName(TEXT("NoCollision"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABossTrigger::BeginOverlap);
}

void ABossTrigger::SetTrigger(bool bActivate)
{
	if (Trigger)
	{
		SetShapeComponent(Trigger, SMARTOBJECT);
	}
}

void ABossTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!BossManager) return;

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		SoulDebug::ScreenLog(__FUNCTION__, FColor::White, 10.f);
		BossManager->IncreaseAreaActor(Player);
	}
}
