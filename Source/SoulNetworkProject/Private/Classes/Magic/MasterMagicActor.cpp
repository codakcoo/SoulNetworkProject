// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Magic/MasterMagicActor.h"
#include "Classes/CharacterBase.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AMasterMagicActor::AMasterMagicActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(GetRootComponent());
	Collision->SetCollisionProfileName("NoCollision");
	Collision->SetIsReplicated(true);

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(Collision);
	Niagara->SetRelativeLocation(FVector(0.f));
	Niagara->SetIsReplicated(true);

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(Collision);


	SetReplicates(true);
}

// Called when the game starts or when spawned
void AMasterMagicActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMasterMagicActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMasterMagicActor, SpellStateType);
	DOREPLIFETIME(AMasterMagicActor, SpawnOwner);
	DOREPLIFETIME(AMasterMagicActor, IgnoreActors);
}

// Called every frame
void AMasterMagicActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!SpawnOwner) return;

	if (SpellStateType == ESpellStateType::Loop)
	{
		
		if (GetLocalRole() == ROLE_Authority)
		{
			UpdatedCollisionEvent(DeltaTime);
		}
		else
		{
			Server_UpdatedCollisionEvent(DeltaTime);
		}
	}
	else if (SpellStateType == ESpellStateType::End)
	{
		UpdatedEndScale(DeltaTime);
	}
}

void AMasterMagicActor::Multicast_BeginData_Implementation()
{
	if (SpellData.BeginNiagaraAsset && SpellData.BeginSoundAsset)
	{
		Niagara->SetAsset(SpellData.BeginNiagaraAsset);
		Niagara->Activate(true);


		Audio->SetSound(SpellData.BeginSoundAsset);
		Audio->Play();
	}
}
bool AMasterMagicActor::Multicast_BeginData_Validate()
{
	return true;
}

void AMasterMagicActor::Multicast_LoopData_Implementation()
{
	if (SpellData.LoopNiagaraAsset && SpellData.LoopSoundAsset)
	{
		Niagara->SetAsset(SpellData.LoopNiagaraAsset);
		Niagara->Activate(true);

		Audio->SetSound(SpellData.LoopSoundAsset);
		Audio->Play();
	}
}
bool AMasterMagicActor::Multicast_LoopData_Validate()
{
	return true;
}

void AMasterMagicActor::Multicast_DetachedSetLocation_Implementation(FVector NewLocation)
{
	SetActorLocation(NewLocation);
	SetActorRotation(FRotator());
}
bool AMasterMagicActor::Multicast_DetachedSetLocation_Validate(FVector NewLocation)
{
	return true;
}

void AMasterMagicActor::PointReaction(FHitResult OutHit)
{
	UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), SpellData.IncreaseValue, OutHit.ImpactPoint, OutHit, SpawnOwner->GetController(), SpawnOwner, MagicDamageType);
	GetWorld()->GetTimerManager().ClearTimer(LifeTimer);
}

void AMasterMagicActor::RadialReaction(FTransform SpawnTransform, float Radius)
{
	UGameplayStatics::ApplyRadialDamage(GetWorld(), SpellData.IncreaseValue, SpawnTransform.GetLocation(), Radius, MagicDamageType, IgnoreActors, SpawnOwner);
	GetWorld()->GetTimerManager().ClearTimer(LifeTimer);
}

void AMasterMagicActor::Server_UpdatedCollisionEvent_Implementation(float InDeltaTime)
{
	UpdatedCollisionEvent(InDeltaTime);
}

void AMasterMagicActor::UpdatedEndScale(float InDeltaTime)
{
	float Scale = GetFloatNiagaraParam(TEXT("Scale"));
	Scale = UKismetMathLibrary::FInterpTo(Scale, 0.f, InDeltaTime, 10.f);
	Niagara->SetFloatParameter(TEXT("Scale"), Scale);

	if(Scale <= 0.0f) Destroy();
}

float AMasterMagicActor::GetFloatNiagaraParam(FName VariableName)
{
	FNiagaraVariable NV(FNiagaraTypeDefinition::GetFloatDef(), VariableName);
	float OutValue;
	Niagara->GetOverrideParameters().GetParameterValue(OutValue, NV);

	return OutValue;
}


void AMasterMagicActor::Multicast_Effect_Implementation(FTransform SpawnTransform)
{
	if (SpellData.ReactionNiagaraAsset && SpellData.ReactionSoundAsset)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpellData.ReactionNiagaraAsset, SpawnTransform.GetLocation(), FRotator(0.f));//SpawnTransform.GetRotation().Rotator());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpellData.ReactionSoundAsset, SpawnTransform.GetLocation());
	}
}
bool AMasterMagicActor::Multicast_Effect_Validate(FTransform SpawnTransform)
{
	return true;
}

