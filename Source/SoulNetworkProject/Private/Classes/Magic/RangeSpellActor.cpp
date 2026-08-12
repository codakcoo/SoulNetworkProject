// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Magic/RangeSpellActor.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/EffectComponent.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

ARangeSpellActor::ARangeSpellActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARangeSpellActor::BeginPlay()
{
	Super::BeginPlay();
}

void ARangeSpellActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARangeSpellActor, TrackChr);
	DOREPLIFETIME(ARangeSpellActor, TrackBoneName);
	DOREPLIFETIME(ARangeSpellActor, LastTrackActor);
	DOREPLIFETIME(ARangeSpellActor, CurrentCount);
}

void ARangeSpellActor::BeginData(ACharacterBase* InOwner, USkeletalMeshComponent* CastMesh)
{
	if (!InOwner) return;
	//const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	SpawnOwner = InOwner;
	//IgnoreActors.Add(SpawnOwner);

	//if (InOwner->GetSpellData().SpellData.SpellType == ESpellType::AttackSorcery || InOwner->GetSpellData().SpellData.SpellType == ESpellType::AssistSorcery)
	//{
	//	if (InOwner->GetMainWeapon().IsSet())
	//	{
	//		Multicast_BeginData();
	//		this->AttachToComponent(SpawnOwner->GetMainWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
	//	}
	//}
	//else if (InOwner->GetSpellData().SpellData.SpellType == ESpellType::AttackIncantation || InOwner->GetSpellData().SpellData.SpellType == ESpellType::AssistIncantation)
	//{
	//	if (InOwner->GetSecondaryWeapon().IsSet())
	//	{
	//		this->AttachToComponent(SpawnOwner->GetSecondaryWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
	//	}
	//}
			Multicast_BeginData();
}

void ARangeSpellActor::LoopData()
{
	if (!SpawnOwner) return;

	//bUseLoop = true;
	SpellStateType = ESpellStateType::Loop;

	if(RangeType == ERangeType::Self) SpawnOwner->GetStateComponent()->AddHealth(SpellData.IncreaseValue);
	if (RangeType == ERangeType::Around)
	{
		const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
		this->DetachFromActor(DetachmentTransformRules);

		if(!bGroudnDetach) SetActorRotation(SpawnOwner->GetActorRotation());
		else if (bGroudnDetach)
		{
			FHitResult OutHit;
			FVector Start = SpawnOwner->GetActorLocation();
			FVector End = Start + FVector(0, 0, -100.f);
			bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, TraceTypeQuery4, false, IgnoreActors, SoulDebug::Trace(), OutHit, true);
			if (bHit)
			{
				Multicast_DetachedSetLocation(OutHit.ImpactPoint + FVector(0.f, 0.f, 5.f));
			}
			else
			{
				Multicast_DetachedSetLocation(OutHit.TraceEnd);
			}
		}

		if (RunType == ERunType::Once)
		{
			if (SpellData.IncreaseValue > 0.0f)
			{
				float Damage = SpellData.IncreaseValue + (SpellData.IncreaseValue * (SpawnOwner->GetStateComponent()->GetINT() / 100));
				SpawnOwner->GetStateComponent()->AddHealth(Damage);
				SpawnOwner->GetEffectComponent()->EffectSequence(EEffectType::Health, FVector::ZeroVector);
			}
			RadialReaction(FTransform(FRotator(), GetActorLocation()), Collision->GetScaledSphereRadius());
		}
	}
	else if (RangeType == ERangeType::Track)
	{
		if (SpawnOwner)
		{
			TrackChr = SpawnOwner->GetFocusActor();
			TrackBoneName = SpawnOwner->GetFocusBoneName();
		}

		Multicast_SetupNiagaraValue();
		

		GetWorld()->GetTimerManager().SetTimer(TrackLoopTimer, FTimerDelegate::CreateWeakLambda(this, [&]
		{
			FHitResult OutHit;
			FVector SpawnLocation = FVector::ZeroVector;
			FVector TargetLoc = FVector::ZeroVector;
			FRotator SpawnRotation = FRotator::ZeroRotator;

			FVector Start;
			FVector End;

			if (CurrentCount == 0)
			{
				Start = SpawnOwner->GetActorLocation() + SpawnOwner->GetActorForwardVector() * 120.0f;// Collision->GetScaledSphereRadius() * 2;
				End = Start + FVector(0.f, 0.f, -200.f);

				if (TrackChr)
				{
					TargetLoc = TrackBoneName == NAME_None ? TrackChr->GetActorLocation() : TrackChr->GetMesh()->GetSocketLocation(TrackBoneName);
					SpawnRotation = FRotator(0.f, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLoc).Yaw, 0.f);// / (TrackCount - CurrentCount), 0.f);
				}
				else
				{
					SpawnRotation = SpawnOwner->GetActorRotation();
				}
			}
			else if (CurrentCount > 0)
			{
				if(!LastTrackActor)
				{
					GetWorld()->GetTimerManager().ClearTimer(TrackLoopTimer);
					return;
				}
				Start = LastTrackActor->GetActorLocation() + LastTrackActor->GetActorForwardVector() * Collision->GetScaledSphereRadius() * 2 + FVector(0.f, 0.f, 100.f);
				End = Start + FVector(0.f, 0.f, -200.f);

				if (TrackChr)
				{
					TargetLoc = TrackBoneName == NAME_None ? TrackChr->GetActorLocation() : TrackChr->GetMesh()->GetSocketLocation(TrackBoneName);
					SpawnRotation = FRotator(0.f, UKismetMathLibrary::FindLookAtRotation(LastTrackActor->GetActorLocation(), TargetLoc).Yaw, 0.f);// / (TrackCount - CurrentCount), 0.f);
				}
				else
				{
					SpawnRotation = LastTrackActor->GetActorRotation();
				}
			}

			bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, TraceTypeQuery4, false, IgnoreActors, SoulDebug::Trace(), OutHit, true);
			if(bHit)
			{
				SpawnLocation = OutHit.ImpactPoint;
			}
			else
			{
				GetWorld()->GetTimerManager().ClearTimer(TrackLoopTimer);
				return;
			}

			LastTrackActor = GetWorld()->SpawnActor<ARangeSpellActor>(this->GetClass(), SpawnLocation, SpawnRotation);
			if (LastTrackActor)
			{
				RadialReaction(FTransform(FRotator(), OutHit.ImpactPoint), Collision->GetScaledSphereRadius());
				if (TrackChr)
				{
					FVector TargetLocNoneZ = FVector(TargetLoc.X, TargetLoc.Y, 0.0f);
					FVector LastTrackLocNoneZ = FVector(LastTrackActor->GetActorLocation().X, LastTrackActor->GetActorLocation().Y, 0.0f);

					if (CurrentCount == TrackCount)
					{
						LastTrackActor->LastReaction(SpawnOwner);

						GetWorld()->GetTimerManager().ClearTimer(TrackLoopTimer);
						return;
					}
					else if ((TargetLocNoneZ - LastTrackLocNoneZ).Size() < Collision->GetScaledSphereRadius())
					{
						LastTrackActor->LastReaction(SpawnOwner);

						GetWorld()->GetTimerManager().ClearTimer(TrackLoopTimer);
						return;
					}
					else
					{
						LastTrackActor->MultipleData(SpawnOwner);
					}
				}
				else
				{
					if (CurrentCount == TrackCount)
					{
						LastTrackActor->LastReaction(SpawnOwner);

						GetWorld()->GetTimerManager().ClearTimer(TrackLoopTimer);
						return;
					}
					else
					{
						LastTrackActor->MultipleData(SpawnOwner);
					}
				}
			}
			CurrentCount++;

		}), TrackLoopTime, true);
	}

	GetWorld()->GetTimerManager().SetTimer(LifeTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	{
		EndData();
	
	}), SpellData.LifeTime, false);
	Multicast_LoopData();
}

void ARangeSpellActor::Multicast_SetupNiagaraValue_Implementation()
{
	Niagara->SetFloatParameter("RangeRadius", Collision->GetScaledSphereRadius());
}
bool ARangeSpellActor::Multicast_SetupNiagaraValue_Validate()
{
	return true;
}

void ARangeSpellActor::MultipleData(ACharacterBase* InOwner)
{
	if (!InOwner) return;

	SpawnOwner = InOwner;
	IgnoreActors.Add(SpawnOwner);
	//bUseLoop = true;
	SpellStateType = ESpellStateType::Loop;

	if (RangeType != ERangeType::Track)
	{
		SpawnOwner->GetStateComponent()->AddMana(-10.f);
	}

	if (RunType == ERunType::Once)
	{
		UGameplayStatics::ApplyRadialDamage(GetWorld(), 
			SpellData.IncreaseValue, 
			GetActorLocation(), 
			Collision->GetScaledSphereRadius(), 
			MagicDamageType, IgnoreActors, 
			SpawnOwner, SpawnOwner->GetController());
	}

	GetWorld()->GetTimerManager().SetTimer(LifeTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	{
		EndData();
	
	}), SpellData.LifeTime, false);
	Multicast_LoopData();
}

void ARangeSpellActor::EndData()
{
	SpawnOwner->Server_ClearSpellActor();

	GetWorld()->GetTimerManager().ClearTimer(LifeTimer);
	Destroy();
}

void ARangeSpellActor::UpdatedCollisionEvent(float InDeltaTime)
{
	if (RunType != ERunType::Duration) return;
	TArray<FHitResult> OutHits;

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorUpVector() * RangeVector.Z;

	bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, Collision->GetScaledSphereRadius() * 2.0f, TraceTypeQuery6, false, IgnoreActors, SoulDebug::Trace(EDrawDebugTrace::ForOneFrame), OutHits, false);
	if (bHit)
	{
		for (auto& OutHit : OutHits)
		{
			if (ACharacterBase* Chr = Cast<ACharacterBase>(OutHit.GetActor()))
			{
				FTransform SpawnTransform;
				SpawnTransform.SetLocation(OutHit.ImpactPoint);

				//Multicast_Reaction(SpawnTransform);
				RadialReaction(SpawnTransform, Collision->GetScaledSphereRadius());
			}
		}
	}
}

/*
void ARangeSpellActor::Multicast_Reaction_Implementation(FTransform SpawnTransform)
{
	UGameplayStatics::ApplyRadialDamage(GetWorld(), -10.f, SpawnTransform.GetLocation(), 1.f, UDamageType::StaticClass(), IgnoreActors);
}
bool ARangeSpellActor::Multicast_Reaction_Validate(FTransform SpawnTransform)
{
	return true;
}
*/

void ARangeSpellActor::LastReaction(ACharacterBase* InOwner)
{
	SpawnOwner = InOwner;
	IgnoreActors.Add(SpawnOwner);
	//bUseLoop = true;
	SpellStateType = ESpellStateType::Loop;


	Multicast_LastReaction();
}

void ARangeSpellActor::Multicast_LastReaction_Implementation()
{	
	if (SpellData.ReactionNiagaraAsset && SpellData.ReactionSoundAsset)
	{
		Niagara->SetAsset(SpellData.ReactionNiagaraAsset);
		Niagara->Activate(true);

		Audio->SetSound(SpellData.ReactionSoundAsset);
		Audio->Play();
	}
}
bool ARangeSpellActor::Multicast_LastReaction_Validate()
{
	return true;
}
