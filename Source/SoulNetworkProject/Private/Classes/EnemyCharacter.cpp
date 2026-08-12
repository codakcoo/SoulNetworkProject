// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/EnemyCharacter.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CombatComponent.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/EffectComponent.h"
#include "Classes/PlayerCharacter.h"
#include "SoulNetworkProject/Blackboard_Keys.h"
#include "Interface/DataInterface.h"


#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

#define FORWARD_SEC 0
#define LEFT_SEC 1
#define RIGHT_SEC 2
#define BACK_SEC 3
#define DODGE_SEC 4

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCombatComponent()->OnDeathEventUpdated.AddDynamic(this, &AEnemyCharacter::EnemyDeathEvent);

	SetReplicates(true);
}


void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	ControllerRef = Cast<AAIControllerBase>(GetController());


	StartLoc = GetActorLocation();
	StartRot = GetActorRotation();
	SetupDataContent();
	Multicast_Refresh();
	Server_RefreshWeaponMesh(GetDead());
	//Multicast_Refresh();
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//UE_LOG(LogTemp, Warning, TEXT("Call RunInterp"));
	if (ControllerRef)
	{
		if (ACharacterBase* Target = Cast<ACharacterBase>(ControllerRef->GetBlackboard()->GetValueAsObject(bb_keys::target_actor)))
		{
			LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
		}

		//if(AnimValues.bLowAttack || AnimValues.bHeavyAttack || AnimValues.bSpellAttack || AnimValues.bAOEAttack)
		{
			//bUseOrientYaw = true;
			RunInterpInputVector(LookRotation);
		}
	}
}

bool AEnemyCharacter::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	static const FName NAME_AILineOfSight = FName(TEXT("TestPawnLineOfSight"));

	FHitResult HitResult;
	FVector SocketLocation = GetMesh()->GetSocketLocation(PerceptionTarget);

	const bool bHitSocket = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, SocketLocation,
		FCollisionObjectQueryParams(ECC_TO_BITFIELD
		(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)),
		FCollisionQueryParams(NAME_AILineOfSight, true,
			IgnoreActor));

	NumberOfLoSChecksPerformed++;

	if (bHitSocket == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
	{
		OutSeenLocation = SocketLocation;
		OutSightStrength = 1;

		return true;
	}

	const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, GetActorLocation(),
		FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) |
			ECC_TO_BITFIELD(ECC_WorldDynamic)), FCollisionQueryParams
			(NAME_AILineOfSight, true, IgnoreActor));

	NumberOfLoSChecksPerformed++;

	if (bHit == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
	{
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;

		return true;
	}


	OutSightStrength = 0;
	return false;
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyCharacter, SpellMap);
	DOREPLIFETIME(AEnemyCharacter, HostileChrs);
}

void AEnemyCharacter::EndInputChecking()
{
	if (AnimValues.bLowAttack)
	{
		if (InputType.bButtonWhenAttack)
		{
			InputType.AllTransFalse();
			AnimValues.AllTransFalse();

			GetHoldTypeWeapon().IsSet() ? Server_E_LowAttackMontage() : Server_LowAttackMontage();
		}
		//else if (InputType.bButtonWhenRoll)
		//{
		//	//CurrentAttackCount = 0;
		//	InputType.AllTransFalse();
		//	AnimValues.AllTransFalse();
		//	//Server_InputValueRollNDodge();
		//	InputValueRollNDodge();
		//}
	}
	else if (AnimValues.bSpellAttack)
	{
		if (InputType.bButtonWhenSpell)
		{
			
			if (SpellData.IsSet())
			{
				AnimValues.AllTransFalse();
				InputType.AllTransFalse();
				Server_E_SpellMontage(SpellData);
			}
			else
			{
				EndMt();
			}
		}
	}
	//else if (AnimValues.bAOEAttack)
	//{
	//	if (InputType.bButtonWhenAttack)
	//	{
	//		EndMt();
	//
	//		if (PressedKey == EPressedKey::Sh_BT) HeavyAttack();
	//		else LowAttack();
	//	}
	//	else if (InputType.bButtonWhenSpell)
	//	{
	//		EndMt();
	//
	//		Server_E_SpellMontage(E_Spell);
	//	}
	//	else if (InputType.bButtonWhenAOE)
	//	{
	//		AnimValues.AllTransFalse();
	//		InputType.AllTransFalse();
	//
	//		AOEAttack();
	//	}
	//	else if (InputType.bButtonWhenRoll)
	//	{
	//		EndMt();
	//		InputValueRollNDodge();
	//	}
	//	else if (InputType.bButtonWhenExpendable)
	//	{
	//		EndMt();
	//		R_KBClicked();
	//	}
	//}
	//else if (AnimValues.bOutofControl)
	//{
	//	if (InputType.bButtonWhenRoll)
	//	{
	//		EndMt();
	//		InputValueRollNDodge();
	//	}
	//}
	//else
	//{
	//	if (InputType.bButtonWhenAttack)
	//	{
	//		EndMt();
	//
	//		if (PressedKey == EPressedKey::Sh_BT) HeavyAttack();
	//		else LowAttack();
	//	}
	//	else if (InputType.bButtonWhenSpell)
	//	{
	//		EndMt();
	//
	//		Server_E_SpellMontage(E_Spell);
	//	}
	//	else if (InputType.bButtonWhenAOE)
	//	{
	//		EndMt();
	//		AOEAttack();
	//	}
	//	else if (InputType.bButtonWhenRoll)
	//	{
	//		EndMt();
	//		InputValueRollNDodge();
	//	}
	//	else if (InputType.bButtonWhenExpendable)
	//	{
	//		EndMt();
	//		R_KBClicked();
	//	}
	//}

	GetCombatComponent()->SetFalseHitSurface();
	
}

void AEnemyCharacter::InputValueRollNDodge()
{
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//if (!AnimInstance) return;
	//
	//if (InputX == 0.f && InputY == 0.f) Server_RollMontage(DODGE_SEC);
	//else
	//{
	//	if (!bUseControllerRotationYaw)
	//	{
	//		FRotator NewRot = UKismetMathLibrary::Conv_VectorToRotator(GetLastMovementInputVector());
	//		SetActorRotation(NewRot);
	//
	//		Server_RollMontage(FORWARD_SEC);
	//	}
	//	else if (bUseControllerRotationYaw)
	//	{
	//		float Degree = UKismetMathLibrary::Atan2(InputX, InputY);
	//		float Yaw_Degreee = Degree * 180 / PI;
	//		//UE_LOG(LogTemp, Warning, TEXT("Yaw_Degress : %1f"), Yaw_Degreee);
	//
	//		FRotator NewRot = UKismetMathLibrary::Conv_VectorToRotator(GetLastMovementInputVector());
	//		SetActorRotation(NewRot);
	//		if (Yaw_Degreee >= -45.f && Yaw_Degreee <= 45.f)
	//		{
	//			Server_RollMontage(FORWARD_SEC);
	//			//UE_LOG(LogTemp, Warning, TEXT("Forward"));
	//		}
	//		else if (Yaw_Degreee < -45.f && Yaw_Degreee >= -130.f)
	//		{
	//			Server_RollMontage(LEFT_SEC);
	//			//UE_LOG(LogTemp, Warning, TEXT("Left"));
	//		}
	//		else if (Yaw_Degreee < -130.f && Yaw_Degreee >= -180.f)
	//		{
	//			Server_RollMontage(BACK_L_SEC);
	//			//UE_LOG(LogTemp, Warning, TEXT("Back_Left"));
	//		}
	//		else if (Yaw_Degreee > 45.f && Yaw_Degreee <= 130.f)
	//		{
	//			Server_RollMontage(RIGHT_SEC);
	//			//UE_LOG(LogTemp, Warning, TEXT("Right"));
	//		}
	//		else if (Yaw_Degreee > 130.f && Yaw_Degreee <= 180.f)
	//		{
	//			Server_RollMontage(BACK_R_SEC);
	//			//UE_LOG(LogTemp, Warning, TEXT("Back_Right"));
	//			//UE_LOG(LogTemp, Warning, TEXT("Back_Right"));
	//		}
	//	}
	//}
}

void AEnemyCharacter::UpdatePointDamage(AActor* DamageCauser)
{
	if (!ControllerRef || !DamageCauser) return;

	if (ACharacterBase* Causer = Cast<ACharacterBase>(DamageCauser))
	{
		if (Causer->GetFaction() != GetFaction())
		{
			if (ControllerRef->GetBlackboard()->GetValueAsEnum(bb_keys::ai_status) != (uint8)EAIState::Attack)
			{
				ControllerRef->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Attack);
			}
			ControllerRef->GetBlackboard()->SetValueAsVector(bb_keys::last_Location, Causer->GetActorLocation());
			ControllerRef->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, DamageCauser);
			ControllerRef->GetBlackboard()->SetValueAsBool(bb_keys::damaged, true);

			SoulDebug::ScreenLog(ControllerRef->GetBlackboard()->GetValueAsBool(bb_keys::damaged) ? TEXT("True") : TEXT("False"), FColor::Purple, 10.f);

			//if(HostileChrs.Num() == 0) HostileChrs.Add(Causer);
			for (auto& Chr : HostileChrs)
			{
				if(Chr == Causer) return;
			}
				HostileChrs.Add(Causer);
		}
	}
	//Con->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, DamageCauser);
	//Con->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Attack);
	//Con->SetFocus(DamageCauser);
	//bUseControllerRotationYaw = true;
	
	
	//Server_OnAttackState(DamageCauser);
}

void AEnemyCharacter::UpdateRadialDamage(AActor* DamageCauser)
{
	if (ControllerRef && DamageCauser)
	{
		ControllerRef->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Investigate);
		SetLookRotation(DamageCauser->GetActorLocation());
	}
}

void AEnemyCharacter::UpdatedExecutionDamage(AActor* DamageCauser)
{
	if (ControllerRef && DamageCauser)
	{
		ControllerRef->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)EAIState::Attack);
		ControllerRef->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)ECombatState::Stun);
		ControllerRef->GetBlackboard()->SetValueAsObject(bb_keys::target_actor, DamageCauser);
	}
}

void AEnemyCharacter::UpdatedHitSurface(FHitResult HitResult, EAttackType AttackType)
{
	GetEffectComponent()->EffectSequence(EEffectType::Surface, HitResult.ImpactPoint, HitResult);
	UpdatedHitEvent(GetActorLocation());

	if (GetCombatComponent())
	{
		GetCombatComponent()->UpdatedCameraShakeArea(HitResult.ImpactPoint, GetCombatComponent()->GetCameraShakeRadiusType(AttackType), AttackType);
	}
}

void AEnemyCharacter::UpdatedSpell(float Value)
{
}

void AEnemyCharacter::UpdatedDeathFunc()
{
	if (GetStateComponent()->GetCurrentHealth() <= 0)
	{
		if (!AnimValues.bOutofControl)
		{
			GetCombatComponent()->Server_RunDeadMotion();
		}
		bDead = true;
	}
}

bool AEnemyCharacter::SetCurrentSpellCount(int8 PermuteValue)
{
	if(PermuteValue < 0 || PermuteValue >= SpellMap.Num()) return false;

	Server_AttachItem(SpellMap[PermuteValue]);

	return true;
}

void AEnemyCharacter::EnemyDeathEvent()
{
	if (GetCombatComponent())
	{
		SetDead(true);
		GetCombatComponent()->Ragdoll();
		
		//UpdatedMainWeaponMesh(true);
		//UpdatedSecondaryWeaponMesh(true);
		//GetMainWeaponMesh()->SetHiddenInGame(true);
		//GetMainShieldMesh()->SetHiddenInGame(true);
		//GetBowWeaponMesh()->SetHiddenInGame(true);
		//GetSecondaryWeaponMesh()->SetHiddenInGame(true);
		//GetSecondaryShieldMesh()->SetHiddenInGame(true);
		UpdatedMainWeaponMesh(true);
		UpdatedSecondaryWeaponMesh(true);
		

		for (auto& Content : DropItems)
		{
			FInventoryData Data = SearchInvenData(this, Content, false);
			if (Data.IsSet())
			{
				FVector CurrentLocation = UpdatedLastLocation();
				if(CurrentLocation == FVector::ZeroVector) break;

				DropItem(Data, CurrentLocation);
			}
		}

		for (auto& Chr : HostileChrs)
		{
			Chr->AddSoul(GetSoul());
		}
		//if (GetLocalRole() == ROLE_Authority)
		//{
		//	//for (auto& Chr : HostileChrs)
		//	//{
		//	//	Chr->AddSoul(GetSoul());
		//	//}
		//
		//}
		//Server_SetSoult();
	}
}

void AEnemyCharacter::Server_SetSoult_Implementation()
{
	for (auto& Chr : HostileChrs)
	{
		Chr->AddSoul(GetSoul());
	}
}

FRotator AEnemyCharacter::GetFocusRotator() const
{
	FRotator FocusRotator = FRotator::ZeroRotator;
	
	if (FocusActor)
	{
		FocusRotator = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), FocusActor->GetActorLocation());
	}
	
	return FocusRotator;
	//return LookRotation;
}

void AEnemyCharacter::SetupCharacter(bool bActivate)
{
	SetDead(bActivate);
	SetupDataContent();


	Multicast_Refresh();
	Server_RefreshWeaponMesh(bActivate);
}

void AEnemyCharacter::SetupObject(bool bActivate)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		SetDead(bActivate);

		SetupDataContent();



		if (GetDead())
		{
			Multicast_Refresh();
			UpdatedMainWeaponMesh(true);
			UpdatedSecondaryWeaponMesh(true);
		}
	}
	else
	{
		Server_SetupCharacter(bActivate);
	}
}
void AEnemyCharacter::Server_SetupCharacter_Implementation(bool bActivate)
{
	SetupCharacter(bActivate);
}

void AEnemyCharacter::RefreshCondition()
{
	SetDead(false);
	UpdatedWeaponMesh();

	SetActorLocation(StartLoc);
	SetActorRotation(StartRot);

	if(GetSpellActor()) GetSpellActor()->Destroy();
	if(GetCombatComponent()) GetCombatComponent()->Ragdoll();
	if(GetStateComponent()) GetStateComponent()->SetupState();
	if(GetEffectComponent()) GetEffectComponent()->AllMeshFadeout(1.0f);
}

void AEnemyCharacter::Multicast_Refresh_Implementation()
{
	if (bDead)
	{
		if (GetCombatComponent())
		{
			GetCombatComponent()->Ragdoll();
			GetCombatComponent()->UpdatedBlockCollision(false);
		}
		if(GetUseFadeMesh()) GetEffectComponent()->AllMeshFadeout(0.0f);

		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

		GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
		AnimValues.AllTransFalse();
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("OverlapPawn"));
		UpdatedMainWeaponMesh(true);
		UpdatedSecondaryWeaponMesh(true);


		if (GetControllerBase())
		{
			GetControllerBase()->GetBrainComponent()->StopLogic("Agent is Dead");
			GetControllerBase()->ClearFocus(EAIFocusPriority::LastFocusPriority);
		}
	}
}

void AEnemyCharacter::Server_RefreshWeaponMesh_Implementation(bool bHiddenMesh)
{
	Multicast_RefreshWeaponMesh(bHiddenMesh);
}

void AEnemyCharacter::Multicast_RefreshWeaponMesh_Implementation(bool bHiddenMesh)
{
	UpdatedMainWeaponMesh(bHiddenMesh);
	UpdatedSecondaryWeaponMesh(bHiddenMesh);
}

float AEnemyCharacter::GetApplyDamge(EEquipHand HandType) const
{
	float ApplyDamageValue = 10.f;

	return ApplyDamageValue;
}

void AEnemyCharacter::SetLookRotation(FVector EventLocation)
{
	if((EventLocation - GetActorLocation()).Size() > MaxLength) return;

	LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), EventLocation);
}

void AEnemyCharacter::Server_EndMt_Implementation()
{
	if (ControllerRef->GetBlackboard()->GetValueAsEnum(bb_keys::combat_state) == (uint8)ECombatState::Stun)
	{
		ControllerRef->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)ECombatState::Strafe);
	}
}

void AEnemyCharacter::SetFocusActor()
{
	if (ControllerRef)
	{
		if (ACharacterBase* Target = Cast<ACharacterBase>(ControllerRef->GetBlackboard()->GetValueAsObject(bb_keys::target_actor)))
		{
			FocusActor = Target;
			FocusBoneName = TEXT("spine_02");
		}
		else
		{
			FocusActor = nullptr;
			FocusBoneName = NAME_None;
		}
	}
}

void AEnemyCharacter::SetupDataContent()
{
	for (auto& Content : DataContents)
	{
		FInventoryData Data = SearchInvenData(this, Content, false);
		Data.EquipIndex = Content.EquipIndex;
		Data.HandType = Content.HandType;
		if (Data.IsSet())
		{
			switch (Data.ItemData.ItemType)
			{
				case EItemType::Weapon:
					Server_AttachItem(Data);
				break;
				case EItemType::Arrow:
					Server_AttachItem(Data);
				break;

				case EItemType::Spell:
					Server_StackSpellData(Data);
				break;
			}
		}
	}
}

void AEnemyCharacter::Server_StackSpellData_Implementation(FInventoryData StackData)
{
	if (StackData.IsSet() && StackData.ItemData.ItemType == EItemType::Spell)
	{
		SpellMap.Add(StackData);
	}
}
bool AEnemyCharacter::Server_StackSpellData_Validate(FInventoryData StackData)
{
	return true;
}

void AEnemyCharacter::EndMt()
{
	SetCanInputCheck(false);

	CurrentLowAttackCount = 0;
	CurrentAOECount = 0;

	AnimValues.AllTransFalse();
	InputType.AllTransFalse();

	IncreaseData.ResetValue();
	SpellData.ResetValue();
	bCharging = false;

	GetStateComponent()->ResetPoiseStat();

	//GetCombatComponent()->AttackType = EAttackType::None;
	GetCombatComponent()->SetFalseHitSurface();
	//GetCombatComponent()->SetExecutionType(EExecutionType::None);

	if (GetLocalRole() == ROLE_Authority)
	{
		Server_EndMt();
	}
	if (ControllerRef)
	{
		//ControllerRef->GetBlackboard()->SetValueAsEnum(bb_keys::ai_status, (uint8)ECombatState::Strafe);
		//Con->GetBlackboard()->SetValueAsEnum(bb_keys::combat_state, (uint8)ECombatState::Strafe);
		//Con->GetBlackboard()->SetValueAsBool(bb_keys::damaged, false);
		//Con->GetBlackboard()->SetValueAsBool(bb_keys::montage, false);
	}
}

void AEnemyCharacter::TestFunction()
{
	if (GetDead())
	{
		if(GetCombatComponent()) GetCombatComponent()->Ragdoll();
	}
}


void AEnemyCharacter::MeleeTrace(EEquipHand HandType)
{
}
