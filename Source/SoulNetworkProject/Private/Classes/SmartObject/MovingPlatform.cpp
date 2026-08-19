// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/MovingPlatform.h"
#include "Classes/CharacterBase.h"

#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Billboard);
	Mesh->SetIsReplicated(true);

	Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button"));
	Button->SetupAttachment(Billboard);
	Button->SetIsReplicated(true);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(Billboard);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMovingPlatform::BeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AMovingPlatform::EndOverlap);
	Trigger->SetIsReplicated(true);

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(Billboard);

	//Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	//Timeline->SetIsReplicated(true);

	PlatformFloat.BindUFunction(this, TEXT("DirectionMovingFloat"));
	PlatformEnd.BindUFunction(this, TEXT("FinishedMovingFloat"));

	ButtonFloat.BindUFunction(this, TEXT("DownButtonFloat"));
	ButtonEnd.BindUFunction(this, TEXT("FinishedDownButton"));

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (ButtonCurve) //MovingfCurve && ButtonCurve)
	{		
		PlatformTimeline.AddInterpFloat(MovingfCurve, PlatformFloat);
		PlatformTimeline.SetTimelineFinishedFunc(PlatformEnd);
		
		ButtonTimeline.AddInterpFloat(ButtonCurve, ButtonFloat);
		ButtonTimeline.SetTimelineFinishedFunc(ButtonEnd);
		


		StartLocation = GetActorLocation();
		EndLocation = GetActorLocation() + TargetLocation;

		ButtonStartLocation = Button->GetRelativeLocation();
		ButtonEndLocation = ButtonStartLocation - FVector(0.f, 0.f, 100.f);

	}
}

void AMovingPlatform::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMovingPlatform, bReverseMoving);
	DOREPLIFETIME(AMovingPlatform, bMoving);
}

void AMovingPlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PlatformTimeline.TickTimeline(DeltaSeconds);
	ButtonTimeline.TickTimeline(DeltaSeconds);
}


void AMovingPlatform::DirectionMovingFloat(float value)
{
	SetActorLocation(bOriginalCondition ? 
		FMath::Lerp(StartLocation, EndLocation, value) :
		FMath::Lerp(EndLocation, StartLocation, value) );
}


void AMovingPlatform::FinishedMovingFloat()
{
	if (EndCue && Audio)
	{
		//bReverseMoving = bReverseMoving ? false : true;
		Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		Audio->Stop();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EndCue, GetActorLocation());

		if (CanOnButton()) ButtonTimeline.Reverse();

		bOriginalCondition = bOriginalCondition ? false : true;

	}
}


void AMovingPlatform::DownButtonFloat(float Value)
{
	Button->SetRelativeLocation(FMath::Lerp(ButtonStartLocation, ButtonEndLocation, Value));
}

void AMovingPlatform::FinishedDownButton()
{
	if(!bMoving)
	{
		bMoving = true;
		PlatformTimeline.PlayFromStart();

		SoulDebug::ScreenLog(__FUNCTION__, FColor::White, 10.f);
	}
	else
	{
		bMoving = false;
	}
}

void AMovingPlatform::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bMoving)
	{
		if (GetVelocity().Size() <= 0.f)
		{
			if (Cast<ACharacterBase>(OtherActor))
			{

				if(ButtonCue && Audio)
				{
					Trigger->SetCollisionProfileName(TEXT("NoCollision"));
					ButtonTimeline.PlayFromStart();

					UGameplayStatics::PlaySoundAtLocation(GetWorld(), ButtonCue, GetActorLocation());
					Audio->Play();
				}
			}
		}
	}
}

void AMovingPlatform::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bMoving)
	{
		if (GetVelocity().Size() <= 0.f)
		{
			if (Cast<ACharacterBase>(OtherActor))
			{
				if (CanOnButton())
				{
					ButtonTimeline.Reverse();
				}
			}
		}
	}
}
/*
void AMovingPlatform::MovingEnd()
{
	if (EndCue && Audio)
	{
		//bReverseMoving = bReverseMoving ? false : true;
		Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EndCue, GetActorLocation());
		Audio->Stop();

		if (CanOnButton()) ButtonTimeline.Reverse();

		bOriginalCondition = bOriginalCondition ? false : true;

	}
}
*/

void AMovingPlatform::SetupObject(bool bCondition)
{
	Super::SetupObject(bCondition);

	bOriginalCondition = bCondition;
	SetActorLocation(bOriginalCondition ? StartLocation : EndLocation);

}

bool AMovingPlatform::CanOnButton()
{
	TArray<AActor*> OverlapActors;
	Trigger->GetOverlappingActors(OverlapActors);

	if(OverlapActors.Num() == 0) return true;

	for (const auto& Chr : OverlapActors)
	{
		if (Cast<ACharacterBase>(Chr)) return false;
	}

	return true;
}
