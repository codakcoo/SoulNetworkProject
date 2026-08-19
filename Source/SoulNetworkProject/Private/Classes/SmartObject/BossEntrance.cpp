// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/BossEntrance.h"
#include "Classes/PlayerCharacter.h"

#include "Interface/GameStateInterface.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameStateBase.h"
#include "SoulNetworkDebug.h"

ABossEntrance::ABossEntrance()
{
	PrimaryActorTick.bCanEverTick = true;

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	WallMesh->SetupAttachment(GetRootComponent());
	WallMesh->SetIsReplicated(true);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	TriggerBox->SetupAttachment(GetRootComponent());
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABossEntrance::BeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABossEntrance::EndOverlap);

	Description = "Traverse the mist";
}

void ABossEntrance::BeginPlay()
{
	Super::BeginPlay();

	//RefreshObject();
}

void ABossEntrance::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	OpacityTimeline.TickTimeline(DeltaSeconds);
}

void ABossEntrance::Interact(APlayerCharacter* Character)
{
	//return nullptr;

	//if(!bDoorActivate || 
	if(!Character) return;

	float Distance = FMath::Abs((GetActorLocation() - Character->GetActorLocation()).Size());

	//TriggerBox->BoxExtent.X
	FVector TargetLoc = Character->GetActorLocation() + GetActorForwardVector() * 100.0f;
	FVector ArrowLoc = Character->GetActorLocation() + GetActorForwardVector() * (-30.0f);
	FHitResult OutHit;
	bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Character->GetActorLocation(), TargetLoc, TraceTypeQuery1, true, {Character}, SoulDebug::Trace(), OutHit, false);
	if (bHit && OutHit.GetActor() && OutHit.GetActor() == this)
	{
		FRotator ArrowRot = UKismetMathLibrary::FindLookAtRotation(ArrowLoc, OutHit.ImpactPoint);

		Character->Server_RunInteraction(this, ArrowLoc, ArrowRot);
		Character->Server_InteractionMontage(EnterAreaMt, NAME_None);

		//Character->Server_EnterArea(ArrowLoc, ArrowRot);


	}
}

FName ABossEntrance::GetInteractableName() const
{
	return Description;
}

void ABossEntrance::UpdatedOverlapEnd()
{
}

void ABossEntrance::PlayInteractionMt()
{


}

void ABossEntrance::SetEntrance(bool bActivate)
{
	bOriginalCondition = bActivate;

	TriggerBox->SetCollisionProfileName(bOriginalCondition ? "SmartObject" : "NoCollision");
	WallMesh->SetCollisionProfileName(bOriginalCondition ? TEXT("BlockAll") : TEXT("NoCollision"));

	TimelineFade(bOriginalCondition ? 1.0f : 0.0f);
}

void ABossEntrance::SetEntranceWithFade(bool bActivate)
{
	bOriginalCondition = bActivate;

	TriggerBox->SetCollisionProfileName(bActivate ? "SmartObject" : "NoCollision");
	WallMesh->SetCollisionProfileName(bActivate ? TEXT("BlockAll") : TEXT("NoCollision"));
	if (bOriginalCondition)
	{
		if (OpacityTimelineReverseCurve)
		{
			FOnTimelineFloat FadeInFunction;
			FadeInFunction.BindUFunction(this, FName("TimelineFade"));
			OpacityTimeline.AddInterpFloat(OpacityTimelineReverseCurve, FadeInFunction);
		}
	}
	else
	{
		if (OpacityTimelineCurve)
		{
			FOnTimelineFloat FadeoutFunction;
			FadeoutFunction.BindUFunction(this, FName("TimelineFade"));
			OpacityTimeline.AddInterpFloat(OpacityTimelineCurve, FadeoutFunction);
		}
	}

	OpacityTimeline.PlayFromStart();
}

void ABossEntrance::RefreshObject()
{
	TriggerBox->SetCollisionProfileName(bOriginalCondition ? "SmartObject" : "NoCollision");
	WallMesh->SetCollisionProfileName(bOriginalCondition ? TEXT("BlockAll") : TEXT("NoCollision"));

	TimelineFade(bOriginalCondition ? 1.0f : 0.0f);
}


void ABossEntrance::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		//UE_LOG(LogTemp, Warning, TEXT("Door Overlap Begin."));
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		bDoorActivate = true;
	}
}

void ABossEntrance::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
		//UE_LOG(LogTemp, Warning, TEXT("Door Overlap End."));
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		bDoorActivate = false;

	}
}

void ABossEntrance::TimelineFade(float Value)
{
	UMaterialInterface* Material = WallMesh->GetMaterial(0);
	UMaterialInstanceDynamic* DynamicMaterial = WallMesh->CreateDynamicMaterialInstance(0, Material);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue("Fadeout", Value);
	}
}
