// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Magic/LaserSpellActor.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

ALaserSpellActor::ALaserSpellActor()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ALaserSpellActor::BeginData(ACharacterBase* InOwner, USkeletalMeshComponent* CastMesh)
{
	if (!InOwner) return;

	//const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	//if (InOwner->GetMainWeapon().IsSet())
	//{
	//	this->AttachToComponent(SpawnOwner->GetMainWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
	//}
	SpawnOwner = InOwner;
	IgnoreActors.Add(SpawnOwner);
	SpellStateType = ESpellStateType::Begin;

	Multicast_BeginData();
}

void ALaserSpellActor::LoopData()
{
	//bUseLoop = true;
	SpellStateType = ESpellStateType::Loop;
	FRotator NewRot(SpawnOwner->GetControlRotation().Pitch, SpawnOwner->GetActorRotation().Yaw, SpawnOwner->GetActorRotation().Roll);
	SetActorRotation(NewRot);

	Multicast_LoopData();
}

void ALaserSpellActor::EndData()
{
	SoulDebug::ScreenLog(__FUNCTION__, FColor::Purple, 10.f);
	SpellStateType = ESpellStateType::End;
	if (SpellData.bDetachAtLoop)//(AttachedVector != FVector::ZeroVector)
	{
		const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
		this->DetachFromActor(DetachmentTransformRules);
	}
	SpawnOwner->Server_ClearSpellActor();
	//this->Destroy();
}

void ALaserSpellActor::UpdatedCollisionEvent(float InDeltaTime)
{

	if (!SpawnOwner) return;


	//if(!bUseLoop) return;

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * CurrentLaserLength;
	FHitResult LengthOutHit;
	TArray<FHitResult> OutHits;

	bool bLenghtHit = GetWorld()->SweepSingleByChannel(LengthOutHit, Start, End, FQuat::Identity, ECC_GameTraceChannel2, FCollisionShape::MakeSphere(Collision->GetScaledSphereRadius()));
	//bool bLenghtHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, Collision->GetScaledSphereRadius(), TraceTypeQuery2, false, {SpawnOwner}, EDrawDebugTrace::ForDuration, LengthOutHit, true);
	if (bLenghtHit)
	{
		//if (LengthOutHit.GetComponent()) 		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Purple, LengthOutHit.GetComponent()->GetName());
		FVector HitLocation = GetActorLocation() - LengthOutHit.ImpactPoint;
	
		float FvectorToX = UKismetMathLibrary::Abs(HitLocation.Size());
		FVector BeamLength = FVector(FvectorToX, 0.f, 0.f);
	
		CurrentLaserLength = FvectorToX;
	
		Multicast_UpdatedBeamEnd(BeamLength);
	
	
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(LengthOutHit.ImpactPoint);
		SpawnTransform.SetRotation(UKismetMathLibrary::MakeRotFromZ(LengthOutHit.ImpactNormal).Quaternion());
	
		Multicast_Effect(SpawnTransform);
	
	}
	else
	{
		CurrentLaserLength = UKismetMathLibrary::FInterpTo(CurrentLaserLength, LaserLength, InDeltaTime, LengthSpeed);

		FVector HitLocation = GetActorLocation() - End;

		float FvectorToX = UKismetMathLibrary::Abs(HitLocation.Size());
		FVector BeamLength = FVector(FvectorToX, 0.f, 0.f);
		Multicast_UpdatedBeamEnd(BeamLength);

		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, __FUNCTION__);

	}
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, Collision->GetScaledSphereRadius(), TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, OutHits, true);
	if (bHit)
	{
		for (FHitResult& OutHit : OutHits)
		{
			if (ACharacterBase* Chr = Cast<ACharacterBase>(OutHit.GetActor()))
			{
				if(SpawnOwner->GetFaction() == Chr->GetFaction()) return;

				FTransform SpawnTransform;
				SpawnTransform.SetLocation(OutHit.ImpactPoint);
				SpawnTransform.SetRotation(UKismetMathLibrary::MakeRotFromZ(OutHit.ImpactNormal).Quaternion());

				Multicast_Effect(SpawnTransform);
				PointReaction(OutHit);
			}
		}
	}
	
}

void ALaserSpellActor::Multicast_UpdatedBeamEnd_Implementation(FVector InBeamLocation)
{
	Niagara->SetVectorParameter("BeamEnd", InBeamLocation);
}
bool ALaserSpellActor::Multicast_UpdatedBeamEnd_Validate(FVector InBeamLocation)
{
	return true;
}
