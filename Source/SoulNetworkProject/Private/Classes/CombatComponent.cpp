// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/CombatComponent.h"

#include "BrainComponent.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/CharacterBase.h"
#include "Classes/AIControllerBase.h"
#include "Classes/EffectComponent.h"
#include "Classes/PlayerControllerBase.h"
#include "Interface/CombatInterface.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

#define TRACE_START TEXT("Trace_Start")
#define TRACE_END TEXT("Trace_End")

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	// ...
	SetIsReplicated(true);
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, bGuardup);
	DOREPLIFETIME(UCombatComponent, bFallingHurt);
	//DOREPLIFETIME(UCombatComponent, CurrentAttacker);
	//DOREPLIFETIME(UCombatComponent, CurrentVictim);
	//DOREPLIFETIME(UCombatComponent, ExecutionType);
	//DOREPLIFETIME(UCombatComponent, VictimChr);
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (GetOwnerRole() == ROLE_Authority)
	{
		UpdatedShieldTrace(DeltaTime);
	}
	else
	{
		Server_UpdatedShieldTrace(DeltaTime);
	}
}

void UCombatComponent::WeaponTrace(EEquipHand HandType, EAttackType AttackType, FName StartSocket, FName EndSocket)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if(!CO) return;
	//if (!Owner) return;

	if (CO->GetLocalRole() == ROLE_Authority)
	{
		float TraceSize = BasicTraceSize;
		TTuple<FVector, FVector> GetLocs;

		if (HandType == EEquipHand::None)
		{
			if (StartSocket == NAME_None)
			{
				CO->MeleeTrace(HandType);
				return;
			}
			else
			{
				GetLocs = GetWeaponLocs(CO->GetMesh(), StartSocket, EndSocket);
			}
		}
		else if (HandType == EEquipHand::Left)
		{
			GetLocs = GetWeaponLocs(CO->GetSecondaryWeapon().WeaponData.WeaponType == EWeaponType::Shield ?
				CO->GetSecondaryShieldMesh() :
				CO->GetSecondaryWeaponMesh(),
				TRACE_START, TRACE_END);

			TraceSize = CO->GetSecondaryWeapon().WeaponData.TraceSize;
		}
		else if (HandType == EEquipHand::Right)
		{
			GetLocs = GetWeaponLocs(CO->GetMainWeapon().WeaponData.WeaponType == EWeaponType::Shield ?
				CO->GetMainShieldMesh() :
				CO->GetMainWeaponMesh(),
				TRACE_START, TRACE_END);


			TraceSize = CO->GetMainWeapon().WeaponData.TraceSize;
		}

		TArray<FHitResult> OutHits;
		//bool bHit = GetWorld()->SweepMultiByChannel(
		//	OutHits, 
		//	GetLocs.Get<0>(), GetLocs.Get<1>(), 
		//	FQuat::Identity, ECC_GameTraceChannel6, FCollisionShape::MakeSphere(TraceSize));
		//if (bShowedDraw) UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), CO->GetMainShieldMesh()->GetComponentLocation(),// GetLocs.Get<0>() +GetLocs.Get<1>(),
		//	(GetLocs.Get<0>() - GetLocs.Get<1>()).Size(), 10.0f, 
		//	UKismetMathLibrary::MakeRotFromX(GetLocs.Get<0>() - GetLocs.Get<1>()), FColor::Green, 5.0f);
		bool bHit = UKismetSystemLibrary::SphereTraceMulti(
			GetWorld(),
			GetLocs.Get<0>(), GetLocs.Get<1>(), TraceSize,
			TraceTypeQuery6, false, {CO},
			SoulDebug::TraceIf(bShowedDraw), OutHits, false);

		if (bHit)
		{

			for (FHitResult& OutHit : OutHits)
			{
				if (SoulDebug::ShouldDraw(bShowedDraw)) UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 30.0f, 12, FColor::Red, 5.0f);

				if (ACharacterBase* Chr = Cast<ACharacterBase>(OutHit.GetActor()))// && Chr->GetFaction() != this->GetFaction())
				{
					
					if(CO == Chr) continue;
					if (CO->GetFaction() != Chr->GetFaction())
					{
						FVector Direction = OutHit.ImpactPoint;

						float DamageValue = 0.f;
						if (CO->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
						{

							DamageValue = Cast<ICombatInterface>(CO)->GetApplyDamge(HandType);
						
						}

						RunCameraShakeAtChr(Chr, AttackType);
						UGameplayStatics::ApplyPointDamage(Chr, DamageValue, Direction, OutHit, CO->GetController(), CO, GetDamageType(AttackType));
					}
				}
				else if(OutHit.PhysMaterial != nullptr)
				{
					if (CO->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
					{
						Cast<ICombatInterface>(CO)->UpdatedHitSurface(OutHit, AttackType);
					}
				}
			}
		}
	}
	else
	{
		Server_WeaponTrace(HandType, AttackType, StartSocket, EndSocket);
	}
}

void UCombatComponent::Server_WeaponTrace_Implementation(EEquipHand HandType, EAttackType AttackType, FName StartSocket, FName EndSocket)
{
	WeaponTrace(HandType, AttackType, StartSocket, EndSocket);
}

//ó���� �ִϸ��̼� �Լ�//
void UCombatComponent::Server_Attacker_Implementation(ACharacterBase* VictimPawn, FName PlaySection)
{
	Multicast_Attacker(VictimPawn, PlaySection);
}

void UCombatComponent::Multicast_Attacker_Implementation(ACharacterBase* VictimPawn, FName PlaySection)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;


	UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !StrikeAttacker_Mt || !VictimPawn || VictimPawn == CO) return;

	CurrentVictim = VictimPawn;
	FString SectionName = PlaySection.ToString();
	CO->AnimValues.bOutofControl = true;
	
	FVector ExecutionLoc = CurrentVictim->GetActorLocation();
	FRotator ExecutionRot = CurrentVictim->GetActorRotation();
	if (PlaySection == TEXT("Forward"))
	{
		ExecutionLoc += CurrentVictim->GetActorForwardVector() * (CurrentVictim->GetCapsuleComponent()->GetScaledCapsuleRadius());

		if (CurrentVictim->GetActorRotation().Yaw + 180.f > 360.f)
		{
			float NewYaw = FMath::Abs((CurrentVictim->GetActorRotation().Yaw + 180.f) - 360.f);
			ExecutionRot = FRotator(0.f, NewYaw, 0.f);
		}
		else
		{
			float NewYaw = FMath::Abs(CurrentVictim->GetActorRotation().Yaw + 180.f);
			ExecutionRot = FRotator(0.f, NewYaw, 0.f);
		}
	}
	else if (PlaySection == TEXT("Behind"))
	{
		ExecutionLoc += CurrentVictim->GetActorForwardVector() * -(CurrentVictim->GetCapsuleComponent()->GetScaledCapsuleRadius());
	}

	CO->SetActorLocation(ExecutionLoc);
	CO->SetActorRotation(ExecutionRot);

	AnimInstance->Montage_Play(StrikeAttacker_Mt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), StrikeAttacker_Mt);
}

////////////////////////

//ó�����ϴ� �ִϸ��̼� ��� �Լ�//
void UCombatComponent::Server_Victim_Implementation(ACharacterBase* AttackerPawn, FName PlaySection)
{
	Multicast_Victim(AttackerPawn, PlaySection);
}

void UCombatComponent::Multicast_Victim_Implementation(ACharacterBase* AttackerPawn, FName PlaySection)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !StrikeVictim_Mt || !AttackerPawn || AttackerPawn == CO) return;

	CurrentAttacker = AttackerPawn;
	FString SectionName = PlaySection.ToString();
	CO->AnimValues.bOutofControl = true;

	AnimInstance->Montage_Play(StrikeVictim_Mt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), StrikeVictim_Mt);
}
/////////////////////////////////

void UCombatComponent::ExecutionDamage()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
		if(!CO) return;

		if (GetCurrentAttacker() && GetCurrentAttacker()->GetCombatComponent()->GetCurrentVictim())
		{

			float DamageValue = 0.f;
			if (CO->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
			{
				DamageValue = Cast<ICombatInterface>(CO)->GetApplyDamge(EEquipHand::None);

			}

			UGameplayStatics::ApplyDamage(
				CO, DamageValue, 
				GetCurrentAttacker()->GetController(), GetCurrentAttacker(), 
				GetCurrentAttacker()->GetCombatComponent()->GetExecutionDamageType());
		}
	}
	//else
	//{
	//	Server_ExecutionDamage();
	//}
}
void UCombatComponent::Server_ExecutionDamage_Implementation()
{
	ExecutionDamage();
}

void UCombatComponent::Server_RunStun_Implementation()
{
	Multicast_RunStun();
}
bool UCombatComponent::Server_RunStun_Validate()
{
	return true;
}

void UCombatComponent::Multicast_RunStun_Implementation()
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !Stun_Mt) return;

	const char* SectionName = "Start";

	CO->AnimValues.bOutofControl = true;
	bOutofControl = true;

	AnimInstance->Montage_Play(Stun_Mt);
	//AnimInstance->Montage_JumpToSection(FName(SectionName), Stun_Mt);
}
bool UCombatComponent::Multicast_RunStun_Validate()
{
	return true;
}

void UCombatComponent::SphereDamageArea(FVector AreaLocation, float AreaRadius, EAttackType AttackType)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	if (GetOwnerRole() == ROLE_Authority)
	{
		//AttackType = AreaDamageType;

		TArray<AActor*> OutActors;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(CO);
		float DamageValue = 0.f;
		if (CO->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			DamageValue = Cast<ICombatInterface>(CO)->GetApplyDamge(CO->GetHoldType());

		}

		UpdatedCameraShakeArea(AreaLocation, AreaRadius, AttackType);
		UGameplayStatics::ApplyRadialDamage(GetWorld(), DamageValue, AreaLocation, AreaRadius, GetDamageType(AttackType), IgnoreActors, CO, CO->GetController());
	}
	else
	{
		Server_SphereDamageArea(AreaLocation, AreaRadius, AttackType);
	}
}



void UCombatComponent::Server_SphereDamageArea_Implementation(FVector AreaLocation, float AreaRadius, EAttackType AttackType)
{
	SphereDamageArea(AreaLocation, AreaRadius, AttackType);
}



void UCombatComponent::UpdatedShieldTrace(float DeltaTime)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	if (CO->GetCharacterMovement()->MaxWalkSpeed > CO->GetWalkSpeed() + 3.f || !CO->GetSecondaryWeapon().IsSet() || CO->IsPlayingMontage())
	{
		//if (CO->AnimValues.IsAttacking())
		//if()
		//{
		//}
			bGuardup = false;
		return;
	}

	if (CO->AnimValues.bGuardUp)
	{
		bGuardup = true;
		return;
	}
	else
	{
		bGuardup = false;
	}
}

void UCombatComponent::Server_UpdatedShieldTrace_Implementation(float DeltaTime)
{
	UpdatedShieldTrace(DeltaTime);
}
bool UCombatComponent::Server_UpdatedShieldTrace_Validate(float DeltaTime)
{
	return true;
}

void UCombatComponent::UpdatedRagdoll(float DeltaTime)
{
}

void UCombatComponent::Server_UpdatedRagdoll_Implementation(float DeltaTime)
{
	UpdatedRagdoll(DeltaTime);
}
bool UCombatComponent::Server_UpdatedRagdoll_Validate(float DeltaTime)
{
	return true;
}



TTuple<FVector, FVector> UCombatComponent::GetWeaponLocs(USkeletalMeshComponent* TraceMesh, FName StartSocketName, FName EndSocketName)
{
	if(!TraceMesh || StartSocketName == NAME_None || EndSocketName == NAME_None) return MakeTuple(FVector(), FVector());

	FVector StartLoc = TraceMesh->GetSocketLocation(StartSocketName);
	FVector EndLoc = TraceMesh->GetSocketLocation(EndSocketName);

	return MakeTuple(StartLoc, EndLoc);
}

void UCombatComponent::UpdatedBlockCollision(bool bActivate)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if(!CO) return;
	
	TArray<UActorComponent*> OutComponents;
	OutComponents = CO->GetComponentsByTag(UShapeComponent::StaticClass(), TEXT("BLOCK"));
	for (UActorComponent* OutComponent : OutComponents)
	{
		if (UShapeComponent* SC = Cast<UShapeComponent>(OutComponent))
		{
			SC->SetCollisionProfileName(bActivate ? TEXT("CharacterMesh") : TEXT("NoCollision"));
		}
	}
}

float UCombatComponent::GetCameraShakeRadiusType(EAttackType AttackType)
{
	float OutputRadius = 0.0f;
	switch (AttackType)
	{
		case EAttackType::Low:
		OutputRadius = LowCameraShakeRadius;
		break;

		case EAttackType::Heavy:
		case EAttackType::Upper:
		OutputRadius = HeavyCameraShakeRadius;
		break;

		default:
		case EAttackType::None:
		break;

	}
	return OutputRadius;
}

void UCombatComponent::Server_SetupCollision_Implementation(bool bActive)
{
	Multicast_SetupCollision(bActive);
}
bool UCombatComponent::Server_SetupCollision_Validate(bool bActive)
{
	return true;
}

void UCombatComponent::Multicast_SetupCollision_Implementation(bool bActive)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	CO->GetCapsuleComponent()->SetCollisionProfileName(bActive ? TEXT("Pawn") : TEXT("OverlapPawn"));
	UpdatedBlockCollision(bActive);
}
bool UCombatComponent::Multicast_SetupCollision_Validate(bool bActive)
{
	return true;
}

void UCombatComponent::Ragdoll()
{
	Server_Ragdoll();
}

void UCombatComponent::Server_Ragdoll_Implementation()
{
	Multicast_Ragdoll();
}
void UCombatComponent::Multicast_Ragdoll_Implementation()
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (CO->GetDead())
	{
		CO->GetMesh()->SetSimulatePhysics(true);
		CO->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

		CO->GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
		CO->AnimValues.AllTransFalse();
		CO->GetCapsuleComponent()->SetCollisionProfileName(TEXT("OverlapPawn"));


		//if (CO->GetControllerBase())
		//{
		//	CO->GetControllerBase()->GetBrainComponent()->StopLogic("Agent is Dead");
		//	CO->GetControllerBase()->ClearFocus(EAIFocusPriority::LastFocusPriority);
		//}
	}
	else
	{
		CO->GetMesh()->SetSimulatePhysics(false);
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		const FVector Position = FVector(0.f, 0.f, -97.f);
		const FRotator Rotation = FRotator(0.f, 270.f, 0.f);
		const FVector Scale = FVector(1.f, 1.f, 1.f);
		FTransform Transform = FTransform(Rotation, Position, Scale);

		CO->GetMesh()->AttachToComponent(CO->GetCapsuleComponent(), AttachmentRules);
		CO->GetMesh()->SetRelativeTransform(Transform);
		CO->GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

		CO->GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
		CO->AnimValues.AllTransFalse();
		CO->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
		UpdatedBlockCollision(true);

		if (CO->GetControllerBase()) CO->GetControllerBase()->GetBrainComponent()->StartLogic();

		//PlayAnimMontage(PlayerMontageData->AnimMontage);
	}
}

void UCombatComponent::UpdatedCameraShakeArea(FVector AreaLocation, float AreaRadius, EAttackType AttackType)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if(!CO) return;

	TArray<AActor*> OutActors;
	if (SoulDebug::ShouldDraw(bShowedDraw)) UKismetSystemLibrary::DrawDebugSphere(GetWorld(), AreaLocation, AreaRadius, 12, FLinearColor(1.f, 1.f, 1.f), 1.0f);
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), AreaLocation, AreaRadius, { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) }, ACharacterBase::StaticClass(), { CO }, OutActors);
	if (bHit)
	{
		//for (auto& OutActor : OutActors)
		//{
		//	if (ACharacterBase* Chr = Cast<ACharacterBase>(OutActor))
		//	{
		//		if(CO->GetFaction() == Chr->GetFaction()) IgnoreActors.Add(Chr);
		//	}
		//}
		RunCameraShakeAtActors(OutActors, AttackType);
	}
}

void UCombatComponent::RunCameraShakeAtChr(ACharacterBase* InChr, EAttackType AttackType)
{
	if (APlayerControllerBase* Con = Cast<APlayerControllerBase>(InChr->GetController()))
	{
		switch (AttackType)
		{
			case EAttackType::None:
			break;

			case EAttackType::Low:
			Con->OneRateCS(EShakeType::LowShake);
			break;

			case EAttackType::Heavy:
			case EAttackType::Upper:
			Con->OneRateCS(EShakeType::HeavyShake);
			break;

			default:
			break;
		}
	}
}
void UCombatComponent::RunCameraShakeAtActors(TArray<AActor*> InActors, EAttackType AttackType)
{
	for (AActor* Actor : InActors)
	{
		if (ACharacterBase* Chr = Cast<ACharacterBase>(Actor))
		{
			RunCameraShakeAtChr(Chr, AttackType);
		}
	}
}

void UCombatComponent::Server_RunDeadMotion_Implementation()
{
	Multicast_RunDeadMotion();
}
bool UCombatComponent::Server_RunDeadMotion_Validate()
{
	return true;
}

void UCombatComponent::Multicast_RunDeadMotion_Implementation()
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !Dead_Mt) return;


	//CO->SetDead(true);
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(Dead_Mt);
	CO->GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	UpdatedBlockCollision(false);

	if (CO->GetControllerBase())
	{
		CO->GetControllerBase()->GetBrainComponent()->StopLogic("Agent is Dead");
		CO->GetControllerBase()->ClearFocus(EAIFocusPriority::LastFocusPriority);
		//CO->GetControllerBase()->GetAIPerceptionComponent()->DestroyComponent(true);
	}
}
bool UCombatComponent::Multicast_RunDeadMotion_Validate()
{
	return true;
}



void UCombatComponent::RunHurtMotion()
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
	if(!AnimInstance || !StrongHurt_Mt || !FallingStrongHurt_Mt) return;


	if(!bFallingHurt)
	{
		SetFallingHurt(true);
		bool bFalling = CO->GetCharacterMovement()->IsFalling();

		if(IsStrongHurt()) Server_HurtMotion(FallingStrongHurt_Mt, FName("Start"));
		else Server_HurtMotion(bFalling ? FallingStrongHurt_Mt : StrongHurt_Mt, FName("Start"));
	}
	else
	{
		SetFallingHurt(false);
		bool bFalling = CO->GetCharacterMovement()->IsFalling();


		Server_HurtMotion(bFalling ? FallingStrongHurt_Mt : StrongHurt_Mt, FName("End"));
	}
}

void UCombatComponent::RunHurtMotion(FRotator InCauserRot)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !LowHurt_Mt) return;

	FString SectionName = FString();

	float CurrentDeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(CO->GetActorRotation(), InCauserRot).Yaw;
	if ((CurrentDeltaYaw >= -180.f && CurrentDeltaYaw < -135.f) || (CurrentDeltaYaw <= 180.f && CurrentDeltaYaw > 135.f))
	{
		SectionName = "Forward";
	}
	else if ((CurrentDeltaYaw <= 135.f && CurrentDeltaYaw > 45.f))
	{
		SectionName = "Right";
	}
	else if ((CurrentDeltaYaw >= -135.f && CurrentDeltaYaw < -45.f))
	{
		SectionName = "Left";
	}
	else if ((CurrentDeltaYaw >= -45.f && CurrentDeltaYaw <= -0.f) || (CurrentDeltaYaw >= 0.f || CurrentDeltaYaw <= 45.f))
	{
		SectionName = "Behind";
	}

	Server_HurtMotion(LowHurt_Mt, FName(SectionName));
	//AnimInstance->Montage_Play(LowHurt_Mt);
	//AnimInstance->Montage_JumpToSection(FName(SectionName), LowHurt_Mt);
}

void UCombatComponent::RunHurtMotion(FVector OriginLoc)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;
	
	UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !StrongHurt_Mt) return;
	
	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(CO->GetActorLocation(), OriginLoc);
	FRotator OptimalRotation = FRotator(0.0f, LookRotation.Yaw, 0.0f);
	Multicast_SetRotation(OptimalRotation);
	//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Blue,__FUNCTION__);

	Server_HurtMotion(StrongHurt_Mt, FName("Start"));
}

void UCombatComponent::RunGuardMotion(EAttackType InType)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !GuardHurt_Mt) return;

	FName SectionName = NAME_None;

	if(InType == EAttackType::Low) SectionName = "Basic";
	if(InType == EAttackType::Heavy || InType == EAttackType::Upper) SectionName = "Strong";


	Server_HurtMotion(GuardHurt_Mt, SectionName);
	//AnimInstance->Montage_Play(GuardHurt_Mt);
	//AnimInstance->Montage_JumpToSection(SectionName, GuardHurt_Mt);
}

void UCombatComponent::Server_HurtMotion_Implementation(UAnimMontage* HurtMt, FName SectionName)
{
	Multicast_HurtMotion(HurtMt, SectionName);
}
bool UCombatComponent::Server_HurtMotion_Validate(UAnimMontage* HurtMt, FName SectionName)
{
	return true;
}

void UCombatComponent::Multicast_HurtMotion_Implementation(UAnimMontage* HurtMt, FName SectionName)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !HurtMt) return;

	CO->AnimValues.bMtMotion = true;

	AnimInstance->Montage_Play(HurtMt);
	AnimInstance->Montage_JumpToSection(SectionName, HurtMt);
}
bool UCombatComponent::Multicast_HurtMotion_Validate(UAnimMontage* HurtMt, FName SectionName)
{
	return true;
}

void UCombatComponent::Server_SetFallingHurt_Implementation(bool bActive)
{
	bFallingHurt = bActive;
}

void UCombatComponent::CallDeathEvent(float Delay)
{
	//Owner->GetCapsuleComponent()->SetCollisionProfileName(TEXT("OutControl"));
	Server_SetupCollision(false);

	GetWorld()->GetTimerManager().SetTimer(DeathEventTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	{
		OnDeathEventUpdated.Broadcast();
	}), Delay, false);
}

void UCombatComponent::Client_CallDeathEvent_Implementation(float Delay)
{
	Server_SetupCollision(false);

	GetWorld()->GetTimerManager().SetTimer(DeathEventTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	{
		OnDeathEventUpdated.Broadcast();
	}), Delay, false);
}
bool UCombatComponent::Client_CallDeathEvent_Validate(float Delay)
{
	return true;
}

void UCombatComponent::SetFallingHurt(bool bActive)
{
	if(GetOwnerRole() == ROLE_Authority) bFallingHurt = bActive;
	else Server_SetFallingHurt(bActive);
}

bool UCombatComponent::IsStrongHurt()
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (CO && StrongHurt_Mt)
	{
		UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
		if (AnimInstance->Montage_IsPlaying(StrongHurt_Mt) && CO->AnimValues.bOutofControl)
		{
			return true;
		}
	}

	return false;
}

bool UCombatComponent::IsFallingWithStrongHurt()
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (CO && FallingStrongHurt_Mt)
	{
		UAnimInstance* AnimInstance = CO->GetMesh()->GetAnimInstance();
		if (AnimInstance->Montage_IsPlaying(FallingStrongHurt_Mt) && CO->AnimValues.bOutofControl)
		{
			return true;
		}
	}

	return false;
}

void UCombatComponent::GetForwardVector(FVector InLocation)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(CO))
	{
		FHitResult OutHit;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(Player);

		FVector Start = Player->GetActorLocation();
		FVector End = Start + Player->GetCamera()->GetForwardVector() * 10000;

		bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 30.f, TraceTypeQuery6, false, IgnoreActors, SoulDebug::Trace(), OutHit, false);
		if (bHit)
		{
			InLocation = OutHit.ImpactPoint;
		}
		else
		{
			InLocation = OutHit.TraceEnd;
		}
	}
}

void UCombatComponent::Client_GetForwardVector_Implementation(FVector InLocation)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO) return;

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(CO))
	{
		FHitResult OutHit;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(Player);

		FVector Start = Player->GetActorLocation();
		FVector End = Start + Player->GetCamera()->GetForwardVector() * 10000;

		bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 30.f, TraceTypeQuery6, false, IgnoreActors, SoulDebug::Trace(), OutHit, false);
		if (bHit)
		{
			InLocation = OutHit.ImpactPoint;
		}
		else
		{
			InLocation = OutHit.TraceEnd;
		}
	}
}
bool UCombatComponent::Client_GetForwardVector_Validate(FVector InLocation)
{
	return true;
}

TSubclassOf<UDamageType> UCombatComponent::GetDamageType(EAttackType AttackType)
{
	switch (AttackType)
	{
	case EAttackType::Low:
	{
		SoulDebug::ScreenLog(__FUNCTION__, FColor::White, 10.0f);
		return LowImpact_C;
	}
	case EAttackType::Heavy:
		return HeavyImpact_C;
	case EAttackType::Upper:
		return UpperImpact_C;
	default:
		return nullptr;
	}

	return nullptr;
}

//void UCombatComponent::SetAttackType(EAttackType InType)
//{
//	AttackType = InType;
//}

void UCombatComponent::SetParryingParam(float DelayTime)
{
	bParrying = true;

	GetWorld()->GetTimerManager().ClearTimer(ParryingTimer);
	GetWorld()->GetTimerManager().SetTimer(ParryingTimer, FTimerDelegate::CreateWeakLambda(this, [&]
		{
			bParrying = false;
		}), DelayTime, false);
}

bool UCombatComponent::CanExecution(class ACharacterBase* InPawn, FName TriggerTag)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if(!CO || !InPawn) return false;

	if (GetHasExecutionAttackerMt() && InPawn->GetCombatComponent()->GetHasExecutionVictimMt())
	{
		TArray<UActorComponent*> TCS = InPawn->GetComponentsByTag(UBoxComponent::StaticClass(), TriggerTag);
		if (TCS.Num() > 0)
		{
			if (UBoxComponent* BoxComp = Cast<UBoxComponent>(TCS[0]))
			{
				TArray<AActor*> OverlapActors;
				BoxComp->GetOverlappingActors(OverlapActors);
				for (auto& OverlapActor : OverlapActors)
				{
					if (ACharacterBase* OverlapPawn = Cast<ACharacterBase>(OverlapActor))
					{
						if (OverlapPawn == CO)
						{

							if (CO->GetFaction() == InPawn->GetFaction())return false;

							FRotator AttackerRot = CO->GetActorRotation();
							FRotator VictimRot = InPawn->GetActorRotation();
							float CurrentDeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(AttackerRot, VictimRot).Yaw;


							if (TriggerTag == TEXT("Forward") && 
								(CurrentDeltaYaw > -180.f && CurrentDeltaYaw < -130.f) || 
								(CurrentDeltaYaw < 180.f && CurrentDeltaYaw > 130.f))
							{
								if ((InPawn->GetCombatComponent()->GetOutofControl() && InPawn->GetCombatComponent()->GetUseForwardTrigger()) ||
									InPawn->GetCombatComponent()->GetAlwaysUseForwardTrigger())
									return true;
							}
							else if (TriggerTag == TEXT("Behind") && 
									(CurrentDeltaYaw < -0.f && CurrentDeltaYaw > -30.f) || 
									(CurrentDeltaYaw > 0.f && CurrentDeltaYaw < 30.f))
							{
								if ((InPawn->GetCombatComponent()->GetOutofControl() && InPawn->GetCombatComponent()->GetUseBehindTrigger()) ||
									InPawn->GetCombatComponent()->GetAlwaysUseBehindTrigger())
									return true;
							}
						}
					
					}
				}
			}
		}
	}

	return false;
}

void UCombatComponent::Multicast_SetLocation_Implementation(FVector NewLocation)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if(!CO || NewLocation == FVector::ZeroVector) return;

	CO->SetActorLocation(NewLocation);
}
void UCombatComponent::Multicast_SetRotation_Implementation(FRotator NewRotation)
{
	ACharacterBase* CO = Cast<ACharacterBase>(GetOwner());
	if (!CO || NewRotation == FRotator::ZeroRotator) return;

	CO->SetActorRotation(NewRotation);
}
