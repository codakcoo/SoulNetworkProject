// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Item/ThrowingKnifeActor.h"
#include "Classes/CharacterBase.h"
#include "Classes/CombatComponent.h"
#include "Classes/ProjectileComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AThrowingKnifeActor::AThrowingKnifeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(RootComponent);
	Collision->SetCollisionProfileName("NoCollision");

	KnifeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("KnifeMesh"));
	KnifeMesh->SetupAttachment(Collision);
	KnifeMesh->SetIsReplicated(true);

	ElementNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElementNiagara"));
	ElementNiagaraComp->SetupAttachment(Collision);
	ElementNiagaraComp->SetIsReplicated(true);

	TrailNigaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailNiagara"));
	TrailNigaraComp->SetupAttachment(Collision);
	TrailNigaraComp->SetIsReplicated(true);

	Projectile = CreateDefaultSubobject<UProjectileComponent>(TEXT("Projectile"));
	Projectile->SetIsReplicated(true);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AThrowingKnifeActor::BeginPlay()
{
	Super::BeginPlay();
	
	//GetWorld()->GetTimerManager().SetTimer(LifeTimer, FTimerDelegate::CreateLambda([&]()
	//{
	//	Destroy();
	//}), LifeTime, false);
}

void AThrowingKnifeActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdatedTrace();
}

void AThrowingKnifeActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AThrowingKnifeActor, Faction);

	DOREPLIFETIME(AThrowingKnifeActor, ApplyOwner);
	DOREPLIFETIME(AThrowingKnifeActor, Damage);
}


void AThrowingKnifeActor::SetupData(FInventoryData Data, ACharacterBase* SetupOwner)
{
	if (!SetupOwner || !Data.IsSet()) Destroy();

	Server_SetupMesh(Data.ItemData.Mesh);
	Damage = Data.ExpendableData.IncreaseValue;

	ApplyOwner = SetupOwner;
	Faction = SetupOwner->GetFaction();

	ElementNiagaraComp->SetWorldLocation(KnifeMesh->GetSocketLocation("Element_Socket"));
	if(Data.ItemData.ItemName == TEXT("FireArrow")) ElementNiagaraComp->SetAsset(FireNiagara);

	TrailNigaraComp->SetWorldLocation(KnifeMesh->GetSocketLocation("Trail_Socket"));
	Projectile->RunProjectile(ApplyOwner);

	//if (HasAuthority())
	//{
	//
	//
	//	//Multicast_RunProjectile();
	//}
	//else
	//{
	//	Server_SetupData(Data, SetupOwner);
	//}
	//if (GetLocalRole() == ROLE_Authority)
	//{
	//}
	//else
	//{
	//	Server_SetupData(Data, SetupOwner);
	//}
}

void AThrowingKnifeActor::Server_SetupMesh_Implementation(USkeletalMesh* Mesh)
{
	Multicast_SetupMesh(Mesh);
}

void AThrowingKnifeActor::Multicast_SetupMesh_Implementation(USkeletalMesh* Mesh)
{
	KnifeMesh->SetSkeletalMesh(Mesh);
}

void AThrowingKnifeActor::UpdatedTrace()
{
	if(!ApplyOwner) return;

	FHitResult HitResult;

	FVector StartLoc = KnifeMesh->GetSocketLocation("Trace_Start");
	FVector EndLoc = KnifeMesh->GetSocketLocation("Trace_End");

	TArray<AActor*> IgnoreActors;

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartLoc, EndLoc, 5.f, TraceTypeQuery6, false, IgnoreActors, EDrawDebugTrace::None, HitResult, false);
	if (bHit)
	{
		if (ACharacterBase* Chr = Cast<ACharacterBase>(HitResult.GetActor()))
		{
			if (Faction != Chr->GetFaction())
			{
				UGameplayStatics::ApplyPointDamage(Chr, Damage, HitResult.ImpactPoint, HitResult, ApplyOwner->GetController(), ApplyOwner, ThrowDamageType_C);

				GetWorld()->GetTimerManager().ClearTimer(LifeTimer);
				Destroy();
			}
		}
		else
		{
			ApplyOwner->GetEffectComponent()->EffectSequence(EEffectType::Surface, HitResult.ImpactPoint, HitResult);

			GetWorld()->GetTimerManager().ClearTimer(LifeTimer);
			Destroy();
		}
	}
}
