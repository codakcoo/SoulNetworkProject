// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Magic/StrikeSpellActor.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

#include "Net/UnrealNetwork.h"

AStrikeSpellActor::AStrikeSpellActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AStrikeSpellActor::BeginPlay()
{
	Super::BeginPlay();
}

void AStrikeSpellActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AStrikeSpellActor, bEndScale);
	DOREPLIFETIME(AStrikeSpellActor, bHitSurface);
	DOREPLIFETIME(AStrikeSpellActor, HitActors);
}

void AStrikeSpellActor::BeginData(ACharacterBase* InOwner, USkeletalMeshComponent* CastMesh)
{
	if (!InOwner) return;
	//const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	SpawnOwner = InOwner;
	IgnoreActors.Add(SpawnOwner);
	SpellStateType = ESpellStateType::Begin;

	Multicast_BeginData();
	Multicast_SetupNiagaraValue();
		//if (InOwner->GetMainWeapon().IsSet())
		//{
		//	this->AttachToComponent(SpawnOwner->GetMainWeaponMesh(), AttachmentTransformRules, "Spell_Socket");
		//
		//}
}

void AStrikeSpellActor::Multicast_SetupNiagaraValue_Implementation()
{
	Niagara->SetFloatParameter("MeshSize", RangeEnd / 100);
	Niagara->SetFloatParameter("Scale", ColorScale);
}

void AStrikeSpellActor::LoopData()
{
	//bUseLoop = true;
	SpellStateType = ESpellStateType::Loop;
}

void AStrikeSpellActor::MultipleData(ACharacterBase* InOwner)
{

}

void AStrikeSpellActor::EndData()
{
	//bUseLoop = false;
	//bEndScale = true;

	SpellStateType = ESpellStateType::End;
	SpawnOwner->Server_ClearSpellActor();

	if (!SpawnOwner->GetStateComponent()->GetDamagePossible())
	{
		if (RangeClass != nullptr)
		{
			AMasterMagicActor* RangeActor = GetWorld()->SpawnActor<AMasterMagicActor>(RangeClass, GetActorLocation(), GetActorRotation());
			if (RangeActor)
			{
				RangeActor->MultipleData(SpawnOwner);
			}
		}
	}

	//FTimerHandle lifeTimer;
	//GetWorld()->GetTimerManager().SetTimer(lifeTimer, this, &AStrikeSpellActor::CallDestroy, SpellData.LifeTime, false);
}

void AStrikeSpellActor::UpdatedCollisionEvent(float InDeltaTime)
{
	//if (bUseLoop && 
	if(RunType == ERunType::Duration)
	{
		TArray<FHitResult> OutHits;

		FVector Start = GetActorLocation();
		FVector End = Start + GetActorUpVector() * (RangeEnd - RangeEnd / 10);

		bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, Collision->GetScaledSphereRadius(), TraceTypeQuery6, false, IgnoreActors, EDrawDebugTrace::None, OutHits, false);
		if (bHit)
		{
			for (auto& OutHit : OutHits)
			{
				if (OutHit.GetActor())
				{
					if (ACharacterBase* Chr = Cast<ACharacterBase>(OutHit.GetActor()))
					{
						if (Chr->GetFaction() == SpawnOwner->GetFaction()) continue;

						FTransform SpawnTransform;
						SpawnTransform.SetLocation(OutHit.ImpactPoint);

						//Multicast_PointReaction(OutHit);
						PointReaction(OutHit);
						bool bPassed = false;
						for (auto& StackChr : HitActors)
						{
							if(StackChr == Chr) bPassed = true;
						}
						if (!bPassed)
						{
							Multicast_Effect(SpawnTransform);
							HitActors.Add(Chr);
						}

						//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Hit Actor"));
					}
				}
				else
				{
					if (!bHitSurface)
					{
						//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green, TEXT("Hit surface"));

						bHitSurface = true;

						FTransform SpawnTransform;
						SpawnTransform.SetLocation(OutHit.ImpactPoint);
						//Multicast_PointReaction(OutHit);

						Multicast_Effect(SpawnTransform);
					}
				}
			}
		}

	}
	//else if (bEndScale)
	//{
	//	Multicast_UpdatedCollisionEvent(InDeltaTime);
	//}
}

void AStrikeSpellActor::Multicast_UpdatedCollisionEvent_Implementation(float InDeltaTime)
{
	ColorScale = UKismetMathLibrary::FInterpTo(ColorScale, 0.f, InDeltaTime, InterpSpeed);
	Niagara->SetFloatParameter("Scale", ColorScale);
}
bool AStrikeSpellActor::Multicast_UpdatedCollisionEvent_Validate(float InDeltaTime)
{
	return true;
}
