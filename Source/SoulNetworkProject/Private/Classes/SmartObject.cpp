// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject.h"

#include "Components/ShapeComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ASmartObject::ASmartObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	//Billboard->SetupAttachment(GetRootComponent());
	RootComponent = Billboard;

	FacingDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("Facing_Direction"));
	FacingDirection->SetupAttachment(Billboard);
}

// Called when the game starts or when spawned
void ASmartObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASmartObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASmartObject, bOriginalCondition);
}

void ASmartObject::SetOriginalCondition(bool bActivate)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bOriginalCondition = bActivate;
	}
	else
	{
		Server_SetOriginalCondition(bActivate);
	}
}
void ASmartObject::Server_SetOriginalCondition_Implementation(bool bActivate)
{
	SetOriginalCondition(bActivate);
}

void ASmartObject::SetShapeComponent(UShapeComponent* CollisionComp, FName CollisionName)
{
	if (CollisionComp)
	{
		CollisionComp->SetCollisionProfileName(CollisionName);
	}
}
