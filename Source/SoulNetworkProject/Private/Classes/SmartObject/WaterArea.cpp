// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/WaterArea.h"
#include "Classes/CharacterBase.h"
#include "Classes/PlayerControllerBase.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/CombatComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AWaterArea::AWaterArea()
{
	WaterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));
	WaterMesh->SetupAttachment(Billboard);
	WaterMesh->SetIsReplicated(true);

	KillZone = CreateDefaultSubobject<UBoxComponent>(TEXT("KillZone"));
	KillZone->SetupAttachment(Billboard);
	KillZone->OnComponentBeginOverlap.AddDynamic(this, &AWaterArea::BeginOverlap);
	KillZone->SetIsReplicated(true);


	SetReplicates(true);
	SetReplicatingMovement(true);
}

void AWaterArea::BeginPlay()
{
	Super::BeginPlay();


}

void AWaterArea::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacterBase* Chr = Cast<ACharacterBase>(OtherActor))
	{
		Chr->GetStateComponent()->AddHealth(-9999.9f);
		Chr->GetEffectComponent()->RunBodyMeshNiagara(TEXT("Dead"));
		Chr->FadeInOut(false);
		Chr->SetDead(true);

		if (APlayerControllerBase* Con = Cast<APlayerControllerBase>(Chr->GetController()))
		{
			Con->TopView();
		}
		Multicast_CallEventOverlapActor(Chr);
	}

}

void AWaterArea::Multicast_CallEventOverlapActor_Implementation(ACharacterBase* Chr)
{
	if(!Chr) return;

	Chr->GetCombatComponent()->CallDeathEvent(1.0f);

}
bool AWaterArea::Multicast_CallEventOverlapActor_Validate(ACharacterBase* Chr)
{
	return true;
}