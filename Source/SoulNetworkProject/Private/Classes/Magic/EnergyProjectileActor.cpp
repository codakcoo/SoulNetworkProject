// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Magic/EnergyProjectileActor.h"
#include "Classes/CharacterBase.h"
#include "Classes/LockonSystemComponent.h"
#include "Classes/ProjectileComponent.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

#include "Net/UnrealNetwork.h"

AEnergyProjectileActor::AEnergyProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;

	//Collision->OnComponentBeginOverlap.AddDynamic(this, &AEnergyProjectileActor::BeginOverlap);
	SpellData.bDetachAtLoop = true;
	ProjectileComponent = CreateDefaultSubobject<UProjectileComponent>(TEXT("Projectile"));
	ProjectileComponent->SetIsReplicated(true);
}

void AEnergyProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	
}

void AEnergyProjectileActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnergyProjectileActor, TrackChr);
	DOREPLIFETIME(AEnergyProjectileActor, TrackBoneName);
	DOREPLIFETIME(AEnergyProjectileActor, bStopMovement);

	//DOREPLIFETIME(AEnergyProjectileActor, Velocity);
	//DOREPLIFETIME(AEnergyProjectileActor, StartLocation);
}

void AEnergyProjectileActor::BeginData(ACharacterBase* InOwner, USkeletalMeshComponent* CastMesh)
{
	if(!InOwner) return;
	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	SpawnOwner = InOwner;
	IgnoreActors.Add(SpawnOwner);
	SpellStateType = ESpellStateType::Begin;

	Multicast_BeginData();

	//if (SpellData.BeginNiagaraAsset && SpellData.BeginSoundAsset)
	//{
	//	Niagara->SetAsset(SpellData.BeginNiagaraAsset);
	//	Niagara->Activate(true);
	//
	//
	//	Audio->SetSound(SpellData.BeginSoundAsset);
	//	Audio->Play();
	//}
	//if (InOwner->GetMainWeapon().IsSet())
	//{
	//	this->AttachToComponent(SpawnOwner->GetMainWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
	//}
}

void AEnergyProjectileActor::LoopData()
{
	const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);

	if (!SpawnOwner)
	{
		Destroy();
		return;
	}

	this->DetachFromActor(DetachmentTransformRules);
	FRotator NewRot(SpawnOwner->GetControlRotation().Pitch, SpawnOwner->GetActorRotation().Yaw, SpawnOwner->GetActorRotation().Roll);
	SetActorRotation(NewRot);
	SpellStateType = ESpellStateType::Loop;
	//bUseLoop = true;
	if (SpawnOwner)
	{
		TrackChr = SpawnOwner->GetFocusActor();
		TrackBoneName = SpawnOwner->GetFocusBoneName();
		ProjectileComponent->RunProjectile(SpawnOwner, SpawnOwner->GetFocusActor(), SpawnOwner->GetFocusBoneName());
	}
	//ProjectileComponent->RunProjectile(SpawnOwner, TrackChr, TrackBoneName);
	//Velocity = GetActorForwardVector() * 1000.0f;
	//StartLocation = GetActorLocation();
	SpawnOwner->Server_ClearSpellActor();

	GetWorld()->GetTimerManager().SetTimer(LifeTimer, FTimerDelegate::CreateWeakLambda(this, [&]
		{
			//EndData();
			Destroy();
		}), SpellData.LifeTime, false);

	//SpawnOwner->GetStateComponent()->AddMana(-10.f);
	//SpawnOwner->Server_ClearSpellActor();
	Multicast_LoopData();
	
}

void AEnergyProjectileActor::MultipleData(ACharacterBase* InOwner)
{
	const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	SpawnOwner = InOwner;
	if (!SpawnOwner)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, __FUNCTION__);

		Destroy();
		return;
	}

	//this->DetachFromActor(DetachmentTransformRules);
	//FRotator NewRot(SpawnOwner->GetControlRotation().Pitch, SpawnOwner->GetActorRotation().Yaw, SpawnOwner->GetActorRotation().Roll);
	//SetActorRotation(NewRot);
	IgnoreActors.Add(SpawnOwner);
	//bUseLoop = true;
	SpellStateType = ESpellStateType::Loop;
	if (SpawnOwner)
	{
		TrackChr = SpawnOwner->GetFocusActor();
		TrackBoneName = SpawnOwner->GetFocusBoneName();
		ProjectileComponent->RunProjectile(SpawnOwner, SpawnOwner->GetFocusActor(), SpawnOwner->GetFocusBoneName());
	}
	//ProjectileComponent->RunProjectile(SpawnOwner, TrackChr, TrackBoneName);
	//Velocity = GetActorForwardVector() * 1000.0f;
	//StartLocation = GetActorLocation();

	GetWorld()->GetTimerManager().SetTimer(LifeTimer, FTimerDelegate::CreateWeakLambda(this, [&]
		{
			EndData();
		}), SpellData.LifeTime, false);

	//SpawnOwner->GetStateComponent()->AddMana(-10.f);
	//SpawnOwner->Server_ClearSpellActor();
	Multicast_LoopData();
	//SpawnOwner->GetStateComponent()->AddMana(-10.f);
}

void AEnergyProjectileActor::EndData()
{
	//bUseLoop = false;
	//bEndScale = true;

	SpellStateType = ESpellStateType::End;
	ProjectileComponent->StopProjectile();
	GetWorld()->GetTimerManager().ClearTimer(LifeTimer);
	GetWorld()->GetTimerManager().SetTimer(LifeTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	{
		Destroy();
	}),2.0f, false);
}

void AEnergyProjectileActor::UpdatedCollisionEvent(float InDeltaTime)
{
	//FHitResult OutHit;

	//RadialReaction(GetActorTransform(), Collision->GetScaledSphereRadius());
	//bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation(), GetActorLocation(), Collision->GetScaledSphereRadius(), TraceTypeQuery6, false, IgnoreActors, EDrawDebugTrace::None, OutHIt, false);
	TArray<FHitResult> OutHits;
	bool bHit = GetWorld()->SweepMultiByChannel(OutHits, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel6, FCollisionShape::MakeSphere(Collision->GetScaledSphereRadius()));
	if (bHit)
	{
		for (FHitResult& OutHit : OutHits)
		{

		if (ACharacterBase* Chr = Cast<ACharacterBase>(OutHit.GetActor()))
		{
			if (Chr->GetFaction() == SpawnOwner->GetFaction()) return;

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(OutHit.Location);
			SpawnTransform.SetRotation(UKismetMathLibrary::MakeRotFromZ(OutHit.ImpactNormal).Quaternion());


			if(RunType == ERunType::Duration) PointReaction(OutHit);
			if (RunType == ERunType::Once)
			{
				PointReaction(OutHit);				
				Multicast_Effect(SpawnTransform);
				Destroy();
			}
		}
		else
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(OutHit.Location);
			SpawnTransform.SetRotation(UKismetMathLibrary::MakeRotFromZ(OutHit.ImpactNormal).Quaternion());


			if (RunType == ERunType::Once || RunType == ERunType::Duration)
			{
				Multicast_Effect(SpawnTransform);
				RadialReaction(SpawnTransform, Collision->GetScaledSphereRadius());

				Destroy();
			}
			else if (RunType == ERunType::Loop)
			{
				ProjectileComponent->StopProjectile();
			}
		}
		}
	}

	UpdatedSpread(InDeltaTime);
}

void AEnergyProjectileActor::UpdatedSpread(float InDeltaTime)
{
	if (CollisionType == ECollisionType::Spread)
	{
		float CurrentRadius = UKismetMathLibrary::FInterpTo(Collision->GetScaledSphereRadius(), MaxRangeRadius, InDeltaTime, RangeInterpSpeed);
		Collision->SetSphereRadius(CurrentRadius);

		Niagara->SetFloatParameter("Range", CurrentRadius);
	}
}



void AEnergyProjectileActor::Multicast_UpdatedCollisionEvent_Implementation(float InDeltaTime)
{
	//if (CollisionType == ECollisionType::Spread)
	//{
	//	float CurrentRadius = UKismetMathLibrary::FInterpTo(Collision->GetScaledSphereRadius(), MaxRangeRadius, InDeltaTime, RangeInterpSpeed);
	//	Collision->SetSphereRadius(CurrentRadius);
	//
	//	Niagara->SetFloatParameter("Range", CurrentRadius);
	//}

	//if (bUseLoop)
	//{
	//}
	//else
	//{
	//	if (bEndScale)
	//	{
	//		ColorScale = UKismetMathLibrary::FInterpTo(ColorScale, 0.f, InDeltaTime, 10.f);
	//		Niagara->SetFloatParameter("Scale", ColorScale);
	//	}
	//}

}
bool AEnergyProjectileActor::Multicast_UpdatedCollisionEvent_Validate(float InDeltaTime)
{
	return true;
}


void AEnergyProjectileActor::Multicast_RadialTrace_Implementation(FTransform SpawnTransform)
{
	UGameplayStatics::ApplyRadialDamage(GetWorld(), SpellData.IncreaseValue, SpawnTransform.GetLocation(), Collision->GetScaledSphereRadius(), MagicDamageType, IgnoreActors, SpawnOwner, SpawnOwner->GetController());
}
bool AEnergyProjectileActor::Multicast_RadialTrace_Validate(FTransform SpawnTransform)
{
	return true;
}
