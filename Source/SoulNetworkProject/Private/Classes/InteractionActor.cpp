// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/InteractionActor.h"
#include "Classes/CharacterBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"


AInteractionActor::AInteractionActor()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(RootComponent);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AInteractionActor::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AInteractionActor::OnOverlapEnd);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);

}

void AInteractionActor::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void AInteractionActor::OnInteration()
{
	bActiveActor = true;
}

void AInteractionActor::OffInteraction()
{
	bActiveActor = false;
}

void AInteractionActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacterBase* Chr = Cast<ACharacterBase>(OtherActor))
	{
		if (Faction == Chr->GetFaction())
		{
			TArray<AActor*> OverlapActors;
			Collision->GetOverlappingActors(OverlapActors);
			/*
			*/
			for (int8 i = 0; i < OverlapActors.Num(); i++)
			{
				if (ACharacterBase* OverlapActor = Cast<ACharacterBase>(OverlapActors[i]))
				{
					if(Faction != OverlapActor->GetFaction())
					{
						OffInteraction();
						return;
					}
				}
			}
			OnInteration();
		}
		else
		{
			OffInteraction();
		}
	}
}

void AInteractionActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacterBase* Chr = Cast<ACharacterBase>(OtherActor))
	{
		OffInteraction();
	}
}
