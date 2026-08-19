// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Magic/MultipleCastSpellActor.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"


AMultipleCastSpellActor::AMultipleCastSpellActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMultipleCastSpellActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMultipleCastSpellActor::BeginData(ACharacterBase* InOwner, USkeletalMeshComponent* CastMesh)
{
	if (!InOwner) return;
	//const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	SpawnOwner = InOwner;
	IgnoreActors.Add(SpawnOwner);
	SpellStateType = ESpellStateType::Begin;
	//if (InOwner->GetMainWeapon().IsSet())
	//{
	//	SetActorLocation(GetActorLocation() + GetActorUpVector() * 90.f);
	//
	//	Multicast_BeginData();
	//	this->AttachToComponent(SpawnOwner->GetMainWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
	//}

	//if (InOwner->GetSpellData().SpellData.SpellType == ESpellType::AttackSorcery || InOwner->GetSpellData().SpellData.SpellType == ESpellType::AssistSorcery)
	//{
	//	if (InOwner->GetMainWeapon().IsSet())
	//	{
	//		Multicast_BeginData();
	//		this->AttachToComponent(SpawnOwner->GetMainWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
	//	}
	//}
	//if (InOwner->GetSpellData().SpellData.SpellType == ESpellType::AttackIncantation || InOwner->GetSpellData().SpellData.SpellType == ESpellType::AssistIncantation)
	//{
	//	if (InOwner->GetSecondaryWeapon().IsSet())
	//	{
	//		Multicast_BeginData();
	//		this->AttachToComponent(SpawnOwner->GetSecondaryWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
	//	}
	//}
	Multicast_BeginData();

}



void AMultipleCastSpellActor::LoopData()
{
	if (!SpawnOwner) return;

	if (SpellData.bDetachAtLoop)//(AttachedVector != FVector::ZeroVector)
	{
		const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
		this->DetachFromActor(DetachmentTransformRules);
	}


	//FVector NewLoc = FVector::ZeroVector;
	//if (MultipleType == EMultipleType::High)
	//{
	//	NewLoc = SpawnOwner->GetActorLocation() + SpawnOwner->GetActorUpVector() * AttachedVector.Z;
	//	SetActorLocation(NewLoc);
	//}
	//else if (MultipleType == EMultipleType::Forward)
	//{
	//	NewLoc = SpawnOwner->GetMainWeaponMesh()->GetSocketLocation("Spell_Socket");
	//	SetActorRotation(SpawnOwner->GetActorRotation());
	//}
	//else if (MultipleType == EMultipleType::Ground)
	//{
	//	NewLoc = SpawnOwner->GetMainWeaponMesh()->GetSocketLocation("Ground_Socket");
	//	SetActorLocation(NewLoc);
	//}
	Multicast_LoopData();


	if (AttachedRotaor != FRotator::ZeroRotator)
	{
		FRotator NewRot(AttachedRotaor.Pitch, SpawnOwner->GetActorRotation().Yaw, SpawnOwner->GetActorRotation().Roll);
		SetActorRotation(NewRot);
	}

	if (SpawnType == ESpawnType::AtOnce)
	{
		for (int8 i = 0; i < SpawnCount; i++)
		{
			SpawnSubSpell();
		}
		//SpawnOwner->Server_ClearSpellActor();
		EndData();
	}
	else if (SpawnType == ESpawnType::Duration)
	{
		GetWorld()->GetTimerManager().SetTimer(LoopTimer, this, &AMultipleCastSpellActor::SpawnSubSpell, LoopTime, true);
	}
}




void AMultipleCastSpellActor::EndData()
{
	const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	this->DetachFromActor(DetachmentTransformRules);
	SpawnOwner->Server_ClearSpellActor();
	SpellStateType = ESpellStateType::End;

	GetWorld()->GetTimerManager().ClearTimer(LoopTimer);
	//Destroy();
}

void AMultipleCastSpellActor::UpdatedCollisionEvent(float InDeltaTime)
{
}

void AMultipleCastSpellActor::SpawnSubSpell()
{
	TTuple<FVector, FRotator> SpawnTransform = GetSpawnTransform();
	//FVector SpawnLocation;
	//FRotator SpawnRotation;


	if (MultipleType == EMultipleType::Ground)
	{
		bool bHit = false;
		while(bHit)
		{
			FHitResult OutHit;
			FVector Start = SpawnTransform.Get<0>() + FVector(0.f, 0.f, 90.f);
			FVector End = Start + FVector(0.f, 0.f, -200.f);
			bool bCheckHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, TraceTypeQuery4, false, IgnoreActors, EDrawDebugTrace::None, OutHit, true);
			if (!bCheckHit)
			{
				bHit = false;
				SpawnTransform.Get<0>() = GetSpawnTransform().Get<0>();
			}
			else
			{
				if (Cast<ACharacterBase>(OutHit.GetActor()) || Cast<AMasterMagicActor>(OutHit.GetActor()))
				{
					bHit = false;
					SpawnTransform.Get<0>() = GetSpawnTransform().Get<0>();
				}
				else
				{
					bHit = true;
					SpawnTransform.Get<0>() = OutHit.ImpactPoint;
				}
			}
		}

	}
	//else if (MultipleType == EMultipleType::High)
	//{
	//
	//}
	AMasterMagicActor* SpawnSpell = GetWorld()->SpawnActor<AMasterMagicActor>(ProjectileSpellClass, SpawnTransform.Get<0>(), SpawnTransform.Get<1>());
	if (SpawnSpell)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, __FUNCTION__);

		SpawnSpell->MultipleData(SpawnOwner);
	}
}

TTuple<FVector, FRotator> AMultipleCastSpellActor::GetSpawnTransform()
{
	FVector SpawnLocation;
	FRotator SpawnRotation;

	if (SpellRandomType == ESpawnRandomType::None)
	{
		SpawnLocation = GetLocalLocation();
		SpawnRotation = GetLocalRotation();
	}
	else if (SpellRandomType == ESpawnRandomType::LocationOnly)
	{
		SpawnLocation = GetRandeomLocation();
		SpawnRotation = GetLocalRotation();
	}
	else if (SpellRandomType == ESpawnRandomType::RotationOnly)
	{
		SpawnLocation = GetLocalLocation();
		SpawnRotation = GetRandomRotation();
	}
	else if (SpellRandomType == ESpawnRandomType::Both)
	{
		SpawnLocation = GetRandeomLocation();
		SpawnRotation = GetRandomRotation();
	}

	return MakeTuple(SpawnLocation, SpawnRotation);
}


FVector AMultipleCastSpellActor::GetLocalLocation()
{
	FVector COLocation = GetActorLocation();
	COLocation += RangeVector;

	return COLocation;
}

FVector AMultipleCastSpellActor::GetRandeomLocation()
{
	float RandomLook = FMath::RandRange(MinRangeLook, MaxRangeLook);
	float RandomTurn = FMath::RandRange(MinRangeTurn, MaxRangeTurn);

	FVector LookVector;
	FVector TurnVector;
	if (MultipleType == EMultipleType::High)
	{
		LookVector = GetActorUpVector() * RandomLook;
		TurnVector = GetActorRightVector() * RandomTurn;
	}
	else if (MultipleType == EMultipleType::Ground)
	{
		LookVector = GetActorForwardVector() * RandomLook;
		TurnVector = GetActorRightVector() * RandomTurn;
	}

	return GetActorLocation() + LookVector + TurnVector;
}

FRotator AMultipleCastSpellActor::GetLocalRotation()
{
	FRotator CORotation = SpawnOwner->GetActorRotation();
	FRotator DeltaRotation = FRotator(RangeRotator.Pitch, RangeRotator.Yaw, 0.0f);
	CORotation += DeltaRotation;

	return CORotation;
}

FRotator AMultipleCastSpellActor::GetRandomRotation()
{
	FRotator CORoatation = SpawnOwner->GetActorRotation();
	float RandomPicth = FMath::RandRange(MinRangePItch, MaxRangePItch);
	float RandomYaw = FMath::RandRange(MinRangeYaw, MaxRangeYaw);

	CORoatation.Pitch += RandomPicth;
	CORoatation.Yaw += RandomYaw;

	return CORoatation;
}

void AMultipleCastSpellActor::EndScale()
{
	//if (bEndScale)
	//{
	//	ColorScale = UKismetMathLibrary::FInterpTo(ColorScale, 0.f, InDeltaTime, 10.f);
	//	Niagara->SetFloatParameter("Scale", ColorScale);
	//}
}
