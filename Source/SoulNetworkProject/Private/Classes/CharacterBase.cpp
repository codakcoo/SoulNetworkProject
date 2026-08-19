// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/CombatComponent.h"
#include "Classes/EffectComponent.h"
#include "Classes/LockonSystemComponent.h"
#include "Classes/AIControllerBase.h"
#include "Classes/PickupActor.h"
#include "Classes/Item/ThrowingKnifeActor.h"
#include "Classes/Magic/MasterMagicActor.h"
#include "Widget/UI/HealthBarWidget.h"
#include "Interface/CombatInterface.h"
#include "Interface/DamageInterface.h"

#include "DamageType/LowImpactDamageType.h"

#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


#include "BrainComponent.h"
#include "Perception/AIPerceptionComponent.h"

#include "Net/UnrealNetwork.h"

#define DISABLE TEXT("NoCollision")
#define ENABLE TEXT("Execution")

#define M_BODYNAME TEXT("/Game/ROG_Modular_Armor/Common/Materials/M_Character_DEMO")
#define M_CLOTHNAME TEXT("/Game/ROG_Modular_Armor/Common/Materials/M_Armor_ORM")

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	static ConstructorHelpers::FClassFinder<AThrowingKnifeActor> ThrowActorClass_C(TEXT("Blueprint'/Game/BP/Pickup/ThrowingKnifeActor_BP.ThrowingKnifeActor_BP_C'"));
	if (ThrowActorClass_C.Succeeded())
	{
		ThrowActorClass = ThrowActorClass_C.Class;
	}


	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 120.0f;

	StC = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("Stat"));
	StC->SetIsReplicated(true);
	CbC = CreateDefaultSubobject<UCombatComponent>(TEXT("CbC"));
	CbC->SetIsReplicated(true);
	EfC = CreateDefaultSubobject<UEffectComponent>(TEXT("EfC"));
	EfC->SetIsReplicated(true);

	static ConstructorHelpers::FClassFinder<UUserWidget> HealthWidget_C(TEXT("WidgetBlueprint'/Game/Widget/UI/HealthWidget_BP.HealthWidget_BP_C'"));

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health_Widget"));
	HealthWidget->SetupAttachment(RootComponent);
	HealthWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidget->SetRelativeLocation(FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 20.f));
	if(HealthWidget_C.Succeeded()) HealthWidget->SetWidgetClass(HealthWidget_C.Class);
	HealthWidget->SetPivot(FVector2D(0.5f, 0.5f));
	if(HealthWidget->GetWidgetClass()) HealthWidget->SetDrawSize(FVector2D(120.0f, 33.0f));
	HealthWidget->SetHiddenInGame(true);

	

	MainWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainWeaponMesh"));
	MainWeaponMesh->SetupAttachment(GetMesh(), "Weapon_r");
	MainWeaponMesh->SetIsReplicated(true);
	MainWeaponMesh->ComponentTags.Add("WEAPON");

	SecondaryWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SecondaryWeaponMesh"));
	SecondaryWeaponMesh->SetupAttachment(GetMesh(), "Weapon_l");
	SecondaryWeaponMesh->SetIsReplicated(true);
	SecondaryWeaponMesh->ComponentTags.Add("WEAPON");

	MainShieldMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainShieldMesh"));	
	MainShieldMesh->SetupAttachment(GetMesh(), "Shield_r");
	MainShieldMesh->SetIsReplicated(true);
	MainShieldMesh->ComponentTags.Add("WEAPON");

	SecondaryShieldMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SecondaryShieldMesh"));
	SecondaryShieldMesh->SetupAttachment(GetMesh(), "Shield_l");
	SecondaryShieldMesh->SetIsReplicated(true);
	SecondaryShieldMesh->ComponentTags.Add("WEAPON");

	HoldWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HoldWeaponMesh"));
	HoldWeaponMesh->SetupAttachment(GetMesh(), "Weapon_h");
	HoldWeaponMesh->SetIsReplicated(true);
	HoldWeaponMesh->ComponentTags.Add("WEAPON");

	HoldShieldMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HoldShieldMesh"));
	HoldShieldMesh->SetupAttachment(GetMesh(), "Shield_h");
	HoldShieldMesh->SetIsReplicated(true);
	HoldShieldMesh->ComponentTags.Add("WEAPON");

	BowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BowMesh"));
	BowMesh->SetupAttachment(GetMesh(), "Bow_l");
	BowMesh->SetIsReplicated(true);
	BowMesh->ComponentTags.Add("WEAPON");

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);


	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	HealthBar = Cast<UHealthBarWidget>(HealthWidget->GetUserWidgetObject());

}

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// 직렬화 손상으로 네이티브 컴포넌트 참조가 비어 있으면 복구한다.
	if (!StC) StC = FindComponentByClass<UCharacterStateComponent>();
	if (!CbC) CbC = FindComponentByClass<UCombatComponent>();
	if (!EfC) EfC = FindComponentByClass<UEffectComponent>();

	if (!EfC)
	{
		UE_LOG(LogSoulGame, Error, TEXT("%s: EffectComponent 복구 실패 — 블루프린트 확인 필요"), *GetName());
	}
}

void ACharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HealthBar)
	{
		HealthBar->OnUpdatedHealthPercent(GetStateComponent()->GetMaxHealth(), GetStateComponent()->GetCurrentHealth());
	}

	
	if (GetLocalRole() == ROLE_Authority)
	{
		SetFocusActor();
		CurrentConRot = GetControlRotation();
	}
	else if(GetRemoteRole() == ROLE_AutonomousProxy)
	{
		Server_SetFocusActor();
		Server_UpdateConRotParam(GetControlRotation());
	}
	OpacityTimeline.TickTimeline(DeltaSeconds);
}

//TakeDamge ����//
float ACharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageValue = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	
	if(bDead) return 0.0f;



	if(MainWeapon.IsSet()) UpdatedWeaponMesh();
	if(GetSpellActor()) Server_EndCastSpell();

	const FPointDamageEvent* PointDamageEvent = (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) ? static_cast<const FPointDamageEvent*>(&DamageEvent) : nullptr;	
	const FRadialDamageEvent* RadialDamageEvent  = (DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) ? static_cast<const FRadialDamageEvent*>(&DamageEvent) : nullptr;

	if (DamageEvent.DamageTypeClass)
	{
		UDamageType* DamageType_obj = Cast<UDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
		if (DamageType_obj->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
		{
			if(PointDamageEvent) Cast<IDamageInterface>(DamageType_obj)->ApplyDamageEffect(DamageCauser, this, PointDamageEvent->ShotDirection, Damage);
			else if (RadialDamageEvent) Cast<IDamageInterface>(DamageType_obj)->ApplyDamageEffect(DamageCauser, this, RadialDamageEvent->Origin, Damage);
			else Cast<IDamageInterface>(DamageType_obj)->ApplyDamageEffect(DamageCauser, this, FVector::ZeroVector, Damage);

		}
	}


	if (GetStateComponent()->GetCurrentHealth() <= 0)
	{
		if (GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			Cast<ICombatInterface>(this)->UpdatedDeathFunc();
			Cast<ICombatInterface>(DamageCauser)->UpdatedDeathFunc();
		}
	}

	return DamageValue;
}
///////////



void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, bDead);

	DOREPLIFETIME(ACharacterBase, Faction);

	DOREPLIFETIME(ACharacterBase, MainWeapon);
	DOREPLIFETIME(ACharacterBase, SecondaryWeapon);

	DOREPLIFETIME(ACharacterBase, E_FirstArrow);
	DOREPLIFETIME(ACharacterBase, E_SecondArrow);

	DOREPLIFETIME(ACharacterBase, FocusActor);
	DOREPLIFETIME(ACharacterBase, FocusBoneName);
	DOREPLIFETIME(ACharacterBase, CurrentSpellActor);

	DOREPLIFETIME(ACharacterBase, CurrentConRot);
}

bool ACharacterBase::IsHostile(ACharacterBase* Chr)
{
	return (Faction != Chr->Faction && Chr->Faction != EFaction::Neutral);
}

void ACharacterBase::DropItem(FInventoryData& DropData, FVector SpawnLocation)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		const FActorSpawnParameters SpawnParameters;
		FTransform SpawnTransform = GetActorTransform();

		SpawnTransform.SetLocation(SpawnLocation);

		APickupActor* SpawnPickupActor = GetWorld()->SpawnActor<APickupActor>(APickupActor::StaticClass(), SpawnTransform);
		if (SpawnPickupActor)
		{
			SpawnPickupActor->Server_SetupContent(DropData);
		}
	}
	else
	{
		Server_DropItem(DropData, SpawnLocation);
	}
}



void ACharacterBase::Server_DropItem_Implementation(FInventoryData DropData, FVector SpawnLocation)
{


	//FVector Start = GetActorLocation();
	//FVector End = Start + GetActorUpVector() * -300.f;
	//FHitResult OutHIt;
	//
	//TArray<AActor*> IgnoreActors;
	//IgnoreActors.Add(this);
	//
	//bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::ForDuration, OutHIt, true);
	//if (bHit)
	//{
	//
	//}
	DropItem(DropData, SpawnLocation);
}
bool ACharacterBase::Server_DropItem_Validate(FInventoryData DropData, FVector SpawnLocation)
{
	return DropData.IsSet();
}

/*
void ACharacterBase::RunDamagePossible(float Delay)
{
	bDamagePossible = false;

	if (GetWorld()->GetTimerManager().IsTimerActive(DamagedHandle)) GetWorld()->GetTimerManager().ClearTimer(DamagedHandle);

	GetWorld()->GetTimerManager().SetTimer(DamagedHandle, FTimerDelegate::CreateLambda([&]()
	{
		bDamagePossible = true;
	}), Delay, false);
}
*/

FInventoryData ACharacterBase::GetHoldTypeWeapon() const
{
	FInventoryData OutputData;

	if(HoldType == EEquipHand::Left) return GetSecondaryWeapon();
	else return GetMainWeapon();

	return OutputData;
}

bool ACharacterBase::IsPlayingMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return true;

	if (AnimInstance->IsAnyMontagePlaying())
	{
		//if(AnimInstance->Montage_IsPlaying(GuardMt)) return false;
		return true;
	}

	return false;
}


FString ACharacterBase::GetSectionType(FInventoryData& UseData)
{
	FString SectionName = "-";
	if (UseData.IsSet())
	{
		if (UseData.ItemData.ItemType == EItemType::Weapon)
		{
			if (AnimValues.bLowAttack)
			{
				SectionName = HoldType == EEquipHand::None ? "1H_" : "2H_";
				SectionName += FString::FromInt(CurrentLowAttackCount);
			}
			else if (AnimValues.bHeavyAttack)
			{
				SectionName = HoldType == EEquipHand::None ? "1H_" : "2H_";
				SectionName += bCharging ? "Charge_End" : "Charge_Start";
			}
			else if (AnimValues.bAOEAttack)
			{
				SectionName = "Combo_";
				SectionName += FString::FromInt(CurrentAOECount);
				//SectionName += bChargeEnd ? 
			}
		}
		else if (UseData.ItemData.ItemType == EItemType::Expendable)
		{
			if (UseData.ExpendableData.ExpendableType == EExpendableType::Potion)
			{
				SectionName = HoldType == EEquipHand::None ? "1H_" : "2H_";
				if(UseData.ItemData.ItemName == "HealthPotion") SectionName += FString::FromInt(0);
				else if(UseData.ItemData.ItemName == "ManaPotion") SectionName += FString::FromInt(1);
			}
			else if (UseData.ExpendableData.ExpendableType == EExpendableType::Throwable)
			{
				if(GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f) SectionName = "Type_2";
				else SectionName = "Type_1";
			}
		}
	}
	else
	{
		if (AnimValues.bLowAttack)
		{
			SectionName = "Combo_" + FString::FromInt(CurrentLowAttackCount);
		}
		else if (AnimValues.bHeavyAttack)
		{
			SectionName = bCharging ? "Charge_End" : "Charge_Start";
			
		}
	}


	return SectionName;
}

void ACharacterBase::OnRep_MainWeapon()
{
	UpdatedWeaponMesh();
}

void ACharacterBase::OnRep_SecondaryWeapon()
{
	UpdatedWeaponMesh();
}

void ACharacterBase::UpdatedWeaponMesh()
{
	GetMainWeaponMesh()->SetHiddenInGame(false);
	GetMainShieldMesh()->SetHiddenInGame(false);
	GetBowWeaponMesh()->SetHiddenInGame(false);
	GetSecondaryWeaponMesh()->SetHiddenInGame(false);
	GetSecondaryShieldMesh()->SetHiddenInGame(false);

	UpdatedMainWeaponMesh();
	UpdatedSecondaryWeaponMesh();
	if (GetEffectComponent()) GetEffectComponent()->RefreshWeaponMesh();
}

void ACharacterBase::UpdatedMainWeaponMesh(bool bHiddenMesh)
{
	if (!MainWeapon.IsSet())
	{
		MainShieldMesh->SetSkeletalMesh(nullptr);
		MainWeaponMesh->SetSkeletalMesh(nullptr);
	}
	else
	{
		if (HoldType == EEquipHand::None)
		{
			if (!bHiddenMesh)
			{
				HoldShieldMesh->SetSkeletalMesh(nullptr);
				HoldWeaponMesh->SetSkeletalMesh(nullptr);

				MainShieldMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? MainWeapon.ItemData.Mesh : nullptr);
				MainWeaponMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? nullptr : MainWeapon.ItemData.Mesh);
			}
			else
			{
				HoldShieldMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? MainWeapon.ItemData.Mesh : nullptr);
				HoldWeaponMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? nullptr : MainWeapon.ItemData.Mesh);

				MainShieldMesh->SetSkeletalMesh(nullptr);
				MainWeaponMesh->SetSkeletalMesh(nullptr);
			}
		}
		else if (HoldType == EEquipHand::Left)
		{
			if (!bHiddenMesh)
			{
				MainShieldMesh->SetSkeletalMesh(nullptr);
				MainWeaponMesh->SetSkeletalMesh(nullptr);

				HoldShieldMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? MainWeapon.ItemData.Mesh : nullptr);
				HoldWeaponMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? nullptr : MainWeapon.ItemData.Mesh);
			}
			else
			{
				MainShieldMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? MainWeapon.ItemData.Mesh : nullptr);
				MainWeaponMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? nullptr : MainWeapon.ItemData.Mesh);

				HoldShieldMesh->SetSkeletalMesh(nullptr);
				HoldWeaponMesh->SetSkeletalMesh(nullptr);
			}
		}
		else if (HoldType == EEquipHand::Right)
		{
			if (MainWeapon.WeaponData.WeaponType == EWeaponType::Bow)
			{
				BowMesh->SetSkeletalMesh(MainWeapon.ItemData.Mesh);

				MainShieldMesh->SetSkeletalMesh(nullptr);
				MainWeaponMesh->SetSkeletalMesh(nullptr);

				return;
			}

			SecondaryShieldMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? MainWeapon.ItemData.Mesh : nullptr);
			MainWeaponMesh->SetSkeletalMesh(MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ? nullptr : MainWeapon.ItemData.Mesh);
		}
	}

	if (GetEffectComponent()) GetEffectComponent()->RefreshWeaponMesh();
}

void ACharacterBase::UpdatedSecondaryWeaponMesh(bool bHiddenMesh)
{
	if(!SecondaryWeapon.IsSet())
	{
		SecondaryShieldMesh->SetSkeletalMesh(nullptr);
		SecondaryWeaponMesh->SetSkeletalMesh(nullptr);
	}
	else
	{
		if (HoldType == EEquipHand::None)
		{
			if (!bHiddenMesh)
			{
				SecondaryShieldMesh->SetSkeletalMesh(SecondaryWeapon.WeaponData.WeaponType != EWeaponType::Shield ? nullptr : SecondaryWeapon.ItemData.Mesh);
				SecondaryWeaponMesh->SetSkeletalMesh(SecondaryWeapon.WeaponData.WeaponType != EWeaponType::Shield ? SecondaryWeapon.ItemData.Mesh : nullptr);

				BowMesh->SetSkeletalMesh(nullptr);
			}
			else
			{
				SecondaryShieldMesh->SetSkeletalMesh(nullptr);
				SecondaryWeaponMesh->SetSkeletalMesh(nullptr);

				BowMesh->SetSkeletalMesh(nullptr);
			}
		}
		else if (HoldType == EEquipHand::Left)
		{

			if (SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Bow)
			{
				BowMesh->SetSkeletalMesh(SecondaryWeapon.ItemData.Mesh);
			
				SecondaryShieldMesh->SetSkeletalMesh(nullptr);
				SecondaryWeaponMesh->SetSkeletalMesh(nullptr);
			
				return;
			}
			SecondaryWeaponMesh->SetSkeletalMesh(nullptr);
			SecondaryShieldMesh->SetSkeletalMesh(SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Shield ? SecondaryWeapon.ItemData.Mesh : nullptr);
			MainWeaponMesh->SetSkeletalMesh(SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Shield ? nullptr : SecondaryWeapon.ItemData.Mesh);
		}
		else if (HoldType == EEquipHand::Right)
		{
			SecondaryWeaponMesh->SetSkeletalMesh(nullptr);
			SecondaryShieldMesh->SetSkeletalMesh(nullptr);
			
			HoldShieldMesh->SetSkeletalMesh(SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Shield ? SecondaryWeapon.ItemData.Mesh : nullptr);
			HoldWeaponMesh->SetSkeletalMesh(SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Shield ? nullptr : SecondaryWeapon.ItemData.Mesh);
		}
	}

	if(GetEffectComponent()) GetEffectComponent()->RefreshWeaponMesh();
}

void ACharacterBase::FadeInOut(bool bReverse)
{
	if (!bReverse)
	{
		if (OpacityTimelineCurve)
		{
			FOnTimelineFloat FadeoutFunction;
			FadeoutFunction.BindUFunction(this, FName("TimelineFade"));
			OpacityTimeline.AddInterpFloat(OpacityTimelineCurve, FadeoutFunction);
		}
	}
	else
	{
		if (OpacityTimelineReverseCurve)
		{
			FOnTimelineFloat FadeInFunction;
			FadeInFunction.BindUFunction(this, FName("TimelineFade"));
			OpacityTimeline.AddInterpFloat(OpacityTimelineReverseCurve, FadeInFunction);
		}
	}

	OpacityTimeline.PlayFromStart();
}

void ACharacterBase::TimelineFade(float Value)
{
	if (GetEffectComponent())
	{
		GetEffectComponent()->AllMeshFadeout(Value);
	}
}



void ACharacterBase::Server_SetFocusActor_Implementation()
{
	SetFocusActor();
}
bool ACharacterBase::Server_SetFocusActor_Validate()
{
	return true;
}

//Input Axis Function//
void ACharacterBase::MoveForward(float Value)
{
	if (!Controller || Value == 0.f || GetCombatComponent()->GetOutofControl()) return;// || bHasCrowdControl) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);

}

void ACharacterBase::MoveRight(float Value)
{
	if (!Controller || Value == 0.f || GetCombatComponent()->GetOutofControl()) return;// || bHasCrowdControl) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);

}

void ACharacterBase::LookUp(float Value)
{
	if(Value == 0.f) return;
	AddControllerPitchInput(Value);
}

void ACharacterBase::Turn(float Value)
{
	if(Value == 0.f) return;
	AddControllerYawInput(Value);
}
//////////////////////

void ACharacterBase::JumpStart()
{
	if(IsPlayingMontage()) return;
	Jump();
}

void ACharacterBase::JumpEnd()
{
	StopJumping();
}

void ACharacterBase::StartCrouch()
{
	if (IsPlayingMontage()) return;
	Server_ToggleCrouch(true);
	Crouch();
}
void ACharacterBase::EndCrouch()
{
	Server_ToggleCrouch(false);
	UnCrouch();
}

void ACharacterBase::Server_ToggleCrouch_Implementation(bool bActive)
{
	Multicast_ToggleCrouch(bActive);
}
void ACharacterBase::Multicast_ToggleCrouch_Implementation(bool bActive)
{
	AnimValues.bCrouch = bActive;
}

void ACharacterBase::ToggleCombat(bool Newbool)
{
	Server_ToggleCombat(Newbool);
	//AnimValues.bCombat = Newbool;
	//bUseControllerRotationYaw = Newbool;
	//GetCharacterMovement()->bOrientRotationToMovement = !Newbool;
}
void ACharacterBase::Server_ToggleCombat_Implementation(bool Newbool)
{
	Multicast_ToggleCombat(Newbool);
}
void ACharacterBase::Multicast_ToggleCombat_Implementation(bool Newbool)
{
	AnimValues.bCombat = Newbool;

}

void ACharacterBase::ToggleADS(bool Newbool)
{
	Server_ToggleADS(Newbool);
	//AnimValues.bADS = Newbool;
}
void ACharacterBase::Server_ToggleADS_Implementation(bool Newbool)
{
	Multicast_ToggleADS(Newbool);
}
void ACharacterBase::Multicast_ToggleADS_Implementation(bool Newbool)
{
	AnimValues.bADS = Newbool;
}


void ACharacterBase::ToggleSpeed(float NewSpeed)
{
	float CurrentSpeed = UKismetMathLibrary::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, NewSpeed, GetWorld()->GetDeltaSeconds(), 25.f);
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;

}


void ACharacterBase::ToggleAim(bool Newbool)
{
	Server_ToggleAim(Newbool);
}

void ACharacterBase::Server_ToggleAim_Implementation(bool Newbool)
{
	Multicast_ToggleAim(Newbool);
}

void ACharacterBase::Multicast_ToggleAim_Implementation(bool Newbool)
{
	AnimValues.bAim = Newbool;
}

void ACharacterBase::Server_AttachItem_Implementation(FInventoryData AttachItem)
{
	if(!AttachItem.IsSet()) return;

	if (AttachItem.ItemData.ItemType == EItemType::Arrow)
	{
		if(AttachItem.EquipIndex == 0)
		{
			E_FirstArrow.PermuteValue(AttachItem);
		}
		else if(AttachItem.EquipIndex == 1)
		{
			E_SecondArrow.PermuteValue(AttachItem);
		}
	}
	else if (AttachItem.ItemData.ItemType == EItemType::Weapon)
	{
		if (AttachItem.HandType == EEquipHand::Left)
		{
			SecondaryWeapon.PermuteValue(AttachItem);
		}
		else if (AttachItem.HandType == EEquipHand::Right)
		{
			MainWeapon.PermuteValue(AttachItem);
		}
	}

	UpdatedWeaponMesh();
}
bool ACharacterBase::Server_AttachItem_Validate(FInventoryData AttachItem)
{
	return true;
}

void ACharacterBase::Server_DetachItem_Implementation(EItemType ItemType, EEquipHand HandType, uint8 EquipIndex)
{
	if (ItemType == EItemType::Arrow)
	{
		if(EquipIndex == 0) E_FirstArrow.ResetValue();
		else if(EquipIndex == 1) E_SecondArrow.ResetValue();
	}
	else if (ItemType == EItemType::Weapon)
	{
		if (HandType == EEquipHand::Left)
		{
			//SecondaryWeaponMesh->SetSkeletalMesh(nullptr);
			//SecondaryShieldMesh->SetSkeletalMesh(nullptr);
			SecondaryWeapon.ResetValue();
		}
		else if (HandType == EEquipHand::Right)
		{
			//MainWeaponMesh->SetSkeletalMesh(nullptr);
			//MainShieldMesh->SetSkeletalMesh(nullptr);
			MainWeapon.ResetValue();
		}
	}

	UpdatedWeaponMesh();
			ToggleAim(false);
			ToggleGuard(false);
}
bool ACharacterBase::Server_DetachItem_Validate(EItemType ItemType, EEquipHand HandType, uint8 EquipIndex)
{
	return true;
}


void ACharacterBase::ToggleGuard(bool Newbool)
{
	Server_ToggleGuard(Newbool);
}

void ACharacterBase::Server_ToggleGuard_Implementation(bool Newbool)
{
	Multicast_ToggleGuard(Newbool);
}
bool ACharacterBase::Server_ToggleGuard_Validate(bool Newbool)
{
	return true;
}

void ACharacterBase::Multicast_ToggleGuard_Implementation(bool Newbool)
{
	AnimValues.bGuardUp = Newbool;
}
bool ACharacterBase::Multicast_ToggleGuard_Validate(bool Newbool)
{
	return true;
}

//LowAttack Func//
void ACharacterBase::Server_LowAttackMontage_Implementation()
{
	Multicast_LowAttackMontage();
}
bool ACharacterBase::Server_LowAttackMontage_Validate()
{
	return true;//!AnimValues.ValueCheckBoolean();
}

void ACharacterBase::Multicast_LowAttackMontage_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !LowAttackMt) return;

	AnimValues.bLowAttack = true;
	

	FString SectionName;

	if (MainWeapon.WeaponData.WeaponType == EWeaponType::Bow)
	{
		SectionName = !AnimValues.bAim ? "Zoom" : "Fire";
		IncreaseData.ResetValue();
		IncreaseData.PermuteValue(E_FirstArrow);
	}
	else
	{
		//SectionName = GetSectionType(MainWeapon);
		SectionName = "Combo_";
		SectionName += FString::FromInt(CurrentLowAttackCount);
	}

	AnimInstance->Montage_Play(LowAttackMt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), LowAttackMt);


	bCharging = false;
	CurrentLowAttackCount++;
	if (CurrentLowAttackCount > MaxLowAttackCount)
	{
		CurrentLowAttackCount = 0;
	}
}
bool ACharacterBase::Multicast_LowAttackMontage_Validate()
{
	return true; //!AnimValues.ValueCheckBoolean();
}

void ACharacterBase::Server_E_LowAttackMontage_Implementation()
{
	Multicast_E_LowAttackMontage();
}
bool ACharacterBase::Server_E_LowAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::Multicast_E_LowAttackMontage_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	AnimValues.bLowAttack = true;
	EWeaponType WeaponType = GetHoldTypeWeapon().WeaponData.WeaponType;
	UAnimMontage* AttackMt = GetHoldTypeWeapon().WeaponData.LowAttackMt;
	uint8 MaxIndex = GetHoldTypeWeapon().WeaponData.MaxLowAttackCount;

	FString SectionName;

	SectionName = HoldType == EEquipHand::None ? "1H_" : "2H_";
	SectionName += FString::FromInt(CurrentLowAttackCount);

	AnimInstance->Montage_Stop(1.f);
	AnimInstance->Montage_Play(AttackMt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), AttackMt);

	bCharging = false;
	CurrentLowAttackCount++;
	IncreaseData.ResetValue();
	
	if (CurrentLowAttackCount > MaxIndex)
	{
		CurrentLowAttackCount = 0;
	}
}
bool ACharacterBase::Multicast_E_LowAttackMontage_Validate()
{
	return true;
}
/////////////////

//HeavyAttack Func//
void ACharacterBase::Server_HeavyAttackMontage_Implementation()
{
	Multicast_HeavyAttackMontage();
}
bool ACharacterBase::Server_HeavyAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::Multicast_HeavyAttackMontage_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !HeavyAttackMt) return;


	//const char* ComboList[] = { "Charge_Start", "Charge_End" };

	AnimValues.bHeavyAttack = true;
	InputType.bButtonWhenAttack = false;


	//FString SectionName = GetSectionType(MainWeapon);
	FString SectionName;// = GetSectionType(HoldType != EEquipHand::Left ? MainWeapon : SecondaryWeapon);//!bCharging ? "Charge_Start" : "Charge_End";
	
	
	
	SectionName = HoldType == EEquipHand::None ? "1H_" : "2H_";
	SectionName += bCharging ? "Charge_End" : "Charge_Start";

	//FName SectionName = TEXT("1H_Charge_Start");



	AnimInstance->Montage_Play(HeavyAttackMt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), HeavyAttackMt);
	CurrentLowAttackCount = 0;
}
bool ACharacterBase::Multicast_HeavyAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::Server_E_HeavyAttackMontage_Implementation()
{
	Multicast_HeavyAttackMontage();
}
bool ACharacterBase::Server_E_HeavyAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::Multicast_E_HeavyAttackMontage_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !HeavyAttackMt) return;


	AnimValues.bHeavyAttack = true;
	InputType.bButtonWhenAttack = false;

	FString SectionName = !bCharging ? "Charge_Start" : "Charge_End";

	AnimInstance->Montage_Play(HeavyAttackMt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), HeavyAttackMt);
	CurrentLowAttackCount = 0;
}
bool ACharacterBase::Multicast_E_HeavyAttackMontage_Validate()
{
	return true;
}
///////////////////////

//AOEAttack Fun//
void ACharacterBase::AOEAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !HeavyAttackMt) return;


	const char* ComboList[] = { "Combo_1", "Combo_2" };

	if (!AnimValues.bHeavyAttack)//!AnimInstance->Montage_IsPlaying(HeavyAttackMt))
	{
		//AnimInstance->Montage_Resume(HeavyAttackMt);
		AnimInstance->Montage_Play(HeavyAttackMt);
		AnimValues.bHeavyAttack = true;
	}
	else if (AnimValues.bHeavyAttack)//AnimInstance->Montage_IsPlaying(HeavyAttackMt))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Heavy Attack"));
		AnimInstance->Montage_Play(HeavyAttackMt);
		AnimInstance->Montage_JumpToSection(FName(ComboList[CurrentAOECount]), HeavyAttackMt);
	}
}
void ACharacterBase::Server_AOEAttackMontage_Implementation()
{
	Multicast_AOEAttackMontage();
}
bool ACharacterBase::Server_AOEAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::Multicast_AOEAttackMontage_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;


	AnimValues.bAOEAttack = true;

	FString SectionName = "Combo_" + FString::FromInt(CurrentAOECount);

	AnimInstance->Montage_Stop(1.f);
	AnimInstance->Montage_Play(AOEMt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), AOEMt);


	bCharging = false;
	CurrentLowAttackCount = 0;
	CurrentAOECount++;

	if (CurrentAOECount > MaxAOECount)
	{
		CurrentAOECount = 0;
	}
}
bool ACharacterBase::Multicast_AOEAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::Server_E_AOEAttackMontage_Implementation()
{
	Multicast_E_AOEAttackMontage();
}
bool ACharacterBase::Server_E_AOEAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::Multicast_E_AOEAttackMontage_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;


	AnimValues.bAOEAttack = true;

	FString SectionName = "Combo_" + FString::FromInt(CurrentAOECount);

	AnimInstance->Montage_Stop(1.f);
	if (HoldType == EEquipHand::None)
	{
		if (SecondaryWeapon.IsSet() && SecondaryWeapon.WeaponData.AOEMt && SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Shield)
		{
			AnimInstance->Montage_Play(SecondaryWeapon.WeaponData.AOEMt);
			AnimInstance->Montage_JumpToSection(FName(SectionName), SecondaryWeapon.WeaponData.AOEMt);
		}
		else if (MainWeapon.IsSet() && MainWeapon.WeaponData.AOEMt)
		{
			AnimInstance->Montage_Play(MainWeapon.WeaponData.AOEMt);
			AnimInstance->Montage_JumpToSection(FName(SectionName), MainWeapon.WeaponData.AOEMt);
		}
		else
		{
			AnimInstance->Montage_Play(AOEMt);
			AnimInstance->Montage_JumpToSection(FName(SectionName), AOEMt);
		}
	}
	else if (HoldType == EEquipHand::Right)
	{
		if (MainWeapon.IsSet() && MainWeapon.WeaponData.AOEMt)
		{
			AnimInstance->Montage_Play(MainWeapon.WeaponData.AOEMt);
			AnimInstance->Montage_JumpToSection(FName(SectionName), MainWeapon.WeaponData.AOEMt);
		}
		else
		{
			AnimInstance->Montage_Play(AOEMt);
			AnimInstance->Montage_JumpToSection(FName(SectionName), AOEMt);
		}
	}
	else if (HoldType == EEquipHand::Left)
	{
		if (SecondaryWeapon.IsSet() && SecondaryWeapon.WeaponData.AOEMt)
		{
			AnimInstance->Montage_Play(SecondaryWeapon.WeaponData.AOEMt);
			AnimInstance->Montage_JumpToSection(FName(SectionName), SecondaryWeapon.WeaponData.AOEMt);
		}
		else
		{
			AnimInstance->Montage_Play(AOEMt);
			AnimInstance->Montage_JumpToSection(FName(SectionName), AOEMt);
		}
	}

	bCharging = false;
	CurrentLowAttackCount = 0;
	CurrentAOECount++;

	if (HoldType == EEquipHand::None)
	{
		if (SecondaryWeapon.IsSet() && SecondaryWeapon.WeaponData.AOEMt && SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Shield)
		{
			if (CurrentAOECount > SecondaryWeapon.WeaponData.MaxHeavyAttackCount)
			{
				CurrentAOECount = 0;
			}
		}
		else if (MainWeapon.IsSet() && MainWeapon.WeaponData.AOEMt)
		{
			if (CurrentAOECount > MainWeapon.WeaponData.MaxHeavyAttackCount)
			{
				CurrentAOECount = 0;
			}
		}
		else
		{
			if (CurrentAOECount > MaxAOECount)
			{
				CurrentAOECount = 0;
			}
		}
	}
	else if (HoldType == EEquipHand::Right)
	{
		if (MainWeapon.IsSet() && MainWeapon.WeaponData.AOEMt)
		{
			if (CurrentAOECount > MainWeapon.WeaponData.MaxHeavyAttackCount)
			{
				CurrentAOECount = 0;
			}
		}
		else
		{
			if (CurrentAOECount > MaxAOECount)
			{
				CurrentAOECount = 0;
			}
		}
	}
	else if (HoldType == EEquipHand::Left)
	{
		if (SecondaryWeapon.IsSet() && SecondaryWeapon.WeaponData.AOEMt)
		{
			if (CurrentAOECount > SecondaryWeapon.WeaponData.MaxHeavyAttackCount)
			{
				CurrentAOECount = 0;
			}
		}
		else
		{
			if (CurrentAOECount > MaxAOECount)
			{
				CurrentAOECount = 0;
			}
		}
	}
}
bool ACharacterBase::Multicast_E_AOEAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::Server_E_SpellMontage_Implementation(FInventoryData InSpellData)
{
	Multicast_E_SpellMontage(InSpellData);
}
bool ACharacterBase::Server_E_SpellMontage_Validate(FInventoryData InSpellData)
{
	return true;
}

void ACharacterBase::Multicast_E_SpellMontage_Implementation(FInventoryData InSpellData)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !InSpellData.IsSet() || InSpellData.ItemData.ItemType != EItemType::Spell) return;

	AnimValues.bSpellAttack = true;

	FString SectionName;
	SpellData.PermuteValue(InSpellData);
	if (SpellData.SpellData.MotionType == ESpellMotionType::Combo)
	{
		SectionName = "Combo_";
		SectionName += FString::FromInt(CurrentLowAttackCount);
	}
	else if (SpellData.SpellData.MotionType == ESpellMotionType::Charge)
	{
		SectionName = !bCharging ? "Start" : "End";
	}
	else if (SpellData.SpellData.MotionType == ESpellMotionType::Repeat)
	{
		SectionName = CurrentLowAttackCount < 1 ? "Start" : "Loop";
	}


	AnimInstance->Montage_Stop(0.f);
	AnimInstance->Montage_Play(InSpellData.SpellData.SpellMotion);
	AnimInstance->Montage_JumpToSection(FName(SectionName), InSpellData.SpellData.SpellMotion);

	if (SpellData.SpellData.MotionType != ESpellMotionType::Charge)
	{
		CurrentLowAttackCount++;

		if(CurrentLowAttackCount > SpellData.SpellData.ComboIndex) CurrentLowAttackCount = 0;
	}
	else
	{
		//if(!bCharging) CurrentLowAttackCount = 0;
	}
}
bool ACharacterBase::Multicast_E_SpellMontage_Validate(FInventoryData InSpellData)
{
	return true;
}

void ACharacterBase::Server_BowMontage_Implementation(bool bPressedShift)
{
	Multicast_BowMontage(bPressedShift);
}
void ACharacterBase::Multicast_BowMontage_Implementation(bool bPressedShift)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	if(GetHoldTypeWeapon().WeaponData.WeaponType != EWeaponType::Bow) return;

	bUseHeavyArrow = bPressedShift;
	AnimValues.bLowAttack = true;
	UAnimMontage* AttackMt = GetHoldTypeWeapon().WeaponData.LowAttackMt;

	FName SectionName = NAME_None;
	SectionName = !AnimValues.bAim ? TEXT("Zoom") : TEXT("Fire");
	//AnimValues.bAim = AnimValues.bAim ? false : true;
	AnimValues.bAim = true;

	AnimInstance->Montage_Stop(1.f);
	AnimInstance->Montage_Play(AttackMt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), AttackMt);

	bCharging = false;
	CurrentLowAttackCount = 0;
}

void ACharacterBase::Server_DualAttackMontage_Implementation()
{
	Multicast_DualAttackMontage();
}
bool ACharacterBase::Server_DualAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::Multicast_DualAttackMontage_Implementation()
{
UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !DualAttackMt) return;

	AnimValues.bDualAttack = true;
	

	FString SectionName;

	SectionName = "Combo_" + FString::FromInt(CurrentDualAttackCount);

	AnimInstance->Montage_Play(DualAttackMt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), DualAttackMt);
	
	bCharging = false;
	CurrentDualAttackCount++;
	if (CurrentDualAttackCount > MaxLowAttackCount)
	{
		CurrentDualAttackCount = 0;
	}
}
bool ACharacterBase::Multicast_DualAttackMontage_Validate()
{
	return true;
}

void ACharacterBase::SpawnCastSpellName(class USkeletalMeshComponent* CastMesh, FName SpellName, FName SpawnSocket)
{
	if (HasAuthority())
	{
		FDataContent Data;
		Data.DataName = SpellName;
		FInventoryData SearchData = SearchInvenData(this, Data, true);

		//if (SearchData.IsSet() && SearchData.ItemData.ItemType == EItemType::Spell)
		{
			//if (!GetSpellData().IsSet()) return;

			FVector SpawnLocation = CastMesh->GetSocketLocation(SpawnSocket);
			FRotator SpawnRotation = GetActorRotation();

			const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
			AMasterMagicActor* SpawnSpell = GetWorld()->SpawnActor<AMasterMagicActor>(SearchData.SpellData.MagicClass, SpawnLocation, SpawnRotation);
			if (SpawnSpell)
			{

				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Purple, __FUNCTION__);
				SpawnSpell->BeginData(this, CastMesh);
				SpawnSpell->AttachToComponent(CastMesh, AttachmentTransformRules, SpawnSocket);

				CurrentSpellActor = SpawnSpell;
			}
		}
	}
	else
	{
		Server_SpawnCastSpellName(CastMesh, SpellName, SpawnSocket);
	}
}

void ACharacterBase::Server_SpawnCastSpellName_Implementation(class USkeletalMeshComponent* CastMesh, FName SpellName, FName SpawnSocket)
{
	SpawnCastSpellName(CastMesh, SpellName, SpawnSocket);
}

void ACharacterBase::SpawnCastSpell()
{
	if (HasAuthority())
	{
		//if (!GetSpellData().IsSet()) return;
		if (!GetSpellData().IsSet()) return;

		USkeletalMeshComponent* CastMesh = nullptr;
		if (GetSpellData().SpellData.SpellType == ESpellType::AttackSorcery || GetSpellData().SpellData.SpellType == ESpellType::AssistSorcery)
		{
			if (GetMainWeapon().IsSet()) CastMesh = GetMainWeaponMesh();
		}
		if (GetSpellData().SpellData.SpellType == ESpellType::AttackIncantation || GetSpellData().SpellData.SpellType == ESpellType::AssistIncantation)
		{
			if (GetSecondaryWeapon().IsSet()) CastMesh = GetSecondaryWeaponMesh();
		}
		
		if (CastMesh)
		{
			FVector SpawnLocation = CastMesh->GetSocketLocation(TEXT("Spell_Socket"));
			FRotator SpawnRotation = GetActorRotation();

			const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
			AMasterMagicActor* SpawnSpell = GetWorld()->SpawnActor<AMasterMagicActor>(GetSpellData().SpellData.MagicClass, SpawnLocation, SpawnRotation);
			if (SpawnSpell)
			{

				SpawnSpell->BeginData(this, CastMesh);
				SpawnSpell->AttachToComponent(CastMesh, AttachmentTransformRules, TEXT("Spell_Socket"));

				CurrentSpellActor = SpawnSpell;
			}
		}
	}
	else
	{
		Server_SpawnCastSpell();
	}
}

void ACharacterBase::Server_SpawnCastSpell_Implementation()
{
	SpawnCastSpell();
}
bool ACharacterBase::Server_SpawnCastSpell_Validate()
{
	return true;
}

void ACharacterBase::Server_LoopCastSpell_Implementation()
{
	if (!CurrentSpellActor) return;

	CurrentSpellActor->LoopData();
}
bool ACharacterBase::Server_LoopCastSpell_Validate()
{
	return true;
}

void ACharacterBase::Server_EndCastSpell_Implementation()
{
	if (!CurrentSpellActor) return;

	CurrentSpellActor->EndData();
}
bool ACharacterBase::Server_EndCastSpell_Validate()
{
	return true;
}

void ACharacterBase::Server_SpawnLowArrowActor_Implementation()
{
	SpawnThrowActor(E_FirstArrow);
}
void ACharacterBase::Server_SpawnHeavyArrowActor_Implementation()
{
	SpawnThrowActor(E_SecondArrow);
}
void ACharacterBase::Server_SpawnExpendableThrowActor_Implementation()
{
	SpawnThrowActor(GetIncreaseData());
}

void ACharacterBase::SpawnThrowActor(FInventoryData ThrowData)
{
	if(!ThrowData.IsSet() || ThrowData.ItemCount == 0) return;

	FVector SpawnLoc = (GetActorLocation() + GetActorForwardVector() * 90.f) + GetActorUpVector() * 30.f;
	FRotator TargetRot = UKismetMathLibrary::MakeRotFromX(GetFocusActor()->GetMesh()->GetSocketLocation(GetFocusBoneName()) - SpawnLoc);
	FRotator SpawnRot = FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.0f);

	AThrowingKnifeActor* ThrowingActor = GetWorld()->SpawnActor<AThrowingKnifeActor>(ThrowActorClass, SpawnLoc, SpawnRot);
	if (ThrowingActor)
	{
		ThrowingActor->SetupData(ThrowData, this);
	}
}

void ACharacterBase::Server_ClearSpellActor_Implementation()
{
	if (CurrentSpellActor)
	{
		CurrentSpellActor = nullptr;
	}
}
bool ACharacterBase::Server_ClearSpellActor_Validate()
{
	return true;
}
///////////////////////

bool ACharacterBase::IsBlockingAttack(AActor* DamageActor)
{
	FVector OwnerVector = GetActorLocation();
	FVector OpponentVector = DamageActor->GetActorLocation();

	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(OwnerVector, OpponentVector);
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), LookRot);

	return false;
}


void ACharacterBase::RollMontage(uint8 DirectionIndex)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(!AnimInstance ||!RollMt) return;

	AnimValues.bRoll = true;

	const char* SectionList[] = { "Forward", "Left", "Right", "Back_L", "Back_R", "Dodge"};
	
	//if (!AnimInstance->Montage_IsPlaying(RollMt))
	//{
	//	AnimInstance->Montage_Play(RollMt);
	//	AnimInstance->Montage_JumpToSection(FName(SectionList[DirectionIndex]), RollMt);
	//}
	//else if (AnimInstance->Montage_IsPlaying(RollMt))
	//{
	//}
	
	AnimInstance->Montage_Play(RollMt);
	AnimInstance->Montage_JumpToSection(FName(SectionList[DirectionIndex]), RollMt);

	Server_RollMontage(DirectionIndex);
}

void ACharacterBase::Server_RollMontage_Implementation(uint8 DirectionIndex)
{
	Multicast_RollMontage(DirectionIndex);
}

bool ACharacterBase::Server_RollMontage_Validate(uint8 DirectionIndex)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !RollMt) return false;
	if(DirectionIndex < 0 || DirectionIndex > 4) return false; 
	

	return true;
}

void ACharacterBase::Multicast_RollMontage_Implementation(uint8 DirectionIndex)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !RollMt) return;

	AnimValues.bRoll = true;

	const char* SectionList[] = { "Forward", "Left", "Right", "Back", "Dodge" };

	/*
	if (!AnimInstance->Montage_IsPlaying(RollMt))
	{
		AnimInstance->Montage_Play(RollMt);
		AnimInstance->Montage_JumpToSection(FName(SectionList[DirectionIndex]), RollMt);
	}
	else if (AnimInstance->Montage_IsPlaying(RollMt))
	{
	}
	*/
	AnimInstance->Montage_Play(RollMt);
	AnimInstance->Montage_JumpToSection(FName(SectionList[DirectionIndex]), RollMt);

	CurrentLowAttackCount = 0;
}
bool ACharacterBase::Multicast_RollMontage_Validate(uint8 DirectionIndex)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !RollMt) return false;
	if (DirectionIndex < 0 || DirectionIndex > 5) return false;


	return true;
}

void ACharacterBase::Server_WeaponChangeMontage_Implementation(float SwitchValue)
{
	Multicast_WeaponChangeMontage(SwitchValue);
}
bool ACharacterBase::Server_WeaponChangeMontage_Validate(float SwitchValue)
{
	return true;
}

void ACharacterBase::Multicast_WeaponChangeMontage_Implementation(float SwitchValue)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !EquipAnimMontage) return;

	
	const char* SectionList[] = {"Left", "Right"};
	FName SectionName = SwitchValue < 0.f ? "Left" : "Right";

	
	AnimInstance->Montage_Play(EquipAnimMontage);
	AnimInstance->Montage_JumpToSection(SectionName, EquipAnimMontage);
}
bool ACharacterBase::Multicast_WeaponChangeMontage_Validate(float SwitchValue)
{
	return true;
}

void ACharacterBase::Server_InteractionMontage_Implementation(class UAnimMontage* PlayMt, FName SectionName)
{
	if (PlayMt)
	{
		Multicast_InteractionMontage(PlayMt, SectionName);
	}
}
void ACharacterBase::Multicast_InteractionMontage_Implementation(class UAnimMontage* PlayMt, FName SectionName)
{
	if (PlayMt)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (!AnimInstance) return;	

		AnimValues.bMtMotion = true;
		AnimInstance->Montage_Play(PlayMt);
		if (SectionName != NAME_None) AnimInstance->Montage_JumpToSection(SectionName, PlayMt);	
	}
}


void ACharacterBase::RunInterpInputVector(FRotator TargetRot)
{
	if (bUseOrientYaw)
	{
		FRotator CurrentRot = GetActorRotation();
		FRotator Target = FRotator(0.f, TargetRot.Yaw, 0.f);


		float Alpha = UKismetMathLibrary::FInterpTo(0.f, 1.f, GetWorld()->GetDeltaSeconds(), 10.f);
		FRotator NewRot = UKismetMathLibrary::RLerp(CurrentRot, Target, 0.1f, true);


		SetActorRotation(NewRot);
	}
	//Server_RunInterpInputVector(TargetRot);
}

void ACharacterBase::Server_RunInterpInputVector_Implementation(FRotator TargetRot)
{
	Multicast_RunInterpInputVector(TargetRot);
}
bool ACharacterBase::Server_RunInterpInputVector_Validate(FRotator TargetRot)
{
	return true;
}

void ACharacterBase::Multicast_RunInterpInputVector_Implementation(FRotator TargetRot)
{
	if (bUseOrientYaw)
	{
		FRotator CurrentRot = GetActorRotation();

		float Alpha = UKismetMathLibrary::FInterpTo(0.f, 1.f, GetWorld()->GetDeltaSeconds(), 20.f);
		FRotator InterpRot = UKismetMathLibrary::RLerp(CurrentRot, TargetRot, Alpha, true);
		FRotator NewRot = FRotator(CurrentRot.Pitch, InterpRot.Yaw, CurrentRot.Roll);

		SetActorRotation(NewRot);
	}
}
bool ACharacterBase::Multicast_RunInterpInputVector_Validate(FRotator TargetRot)
{
	return true;
}




void ACharacterBase::Server_SetUseOrientYaw_Implementation(bool bActive)
{
	Multicast_SetUseOrientYaw(bActive);
}
bool ACharacterBase::Server_SetUseOrientYaw_Validate(bool bActive)
{
	return true;
}

void ACharacterBase::Multicast_SetUseOrientYaw_Implementation(bool bActive)
{
	bUseOrientYaw = bActive;
}
bool ACharacterBase::Multicast_SetUseOrientYaw_Validate(bool bActive)
{
	return true;
}

//Ȧ�� Ÿ�� �����ϱ�//
void ACharacterBase::Server_UpdatedHandType_Implementation(EEquipHand PermuteHandType)
{
	Multicast_UpdatedHandType(PermuteHandType);
}
bool ACharacterBase::Server_UpdatedHandType_Validate(EEquipHand PermuteHandType)
{
	return true;
}

void ACharacterBase::Multicast_UpdatedHandType_Implementation(EEquipHand PermuteHandType)
{
	HoldType = HoldType != EEquipHand::None ? EEquipHand::None : PermuteHandType;
	//AnimValues.bTwoHand = HoldType == EEquipHand::None ? false : true;

	UpdatedWeaponMesh();

}
bool ACharacterBase::Multicast_UpdatedHandType_Validate(EEquipHand PermuteHandType)
{
	return true;
}
////////////////////

void ACharacterBase::SetFaction(EFaction InFaction)
{
	Faction = InFaction;
	//Client_SetFaction(InFaction);
}

void ACharacterBase::Client_SetFaction_Implementation(EFaction InFaction)
{
	//Faction = InFaction;
	Server_SetFaction(InFaction);

}
bool ACharacterBase::Client_SetFaction_Validate(EFaction InFaction)
{
	if ((int8)InFaction < 0 || (int8)InFaction > 3) return false;
	return true;
}

void ACharacterBase::Server_SetFaction_Implementation(EFaction InFaction)
{
	Faction = InFaction;
}

bool ACharacterBase::Server_SetFaction_Validate(EFaction InFaction)
{
	if((int8)InFaction < 0 || (int8)InFaction > 3) return false;
	return true;
}

void ACharacterBase::Server_UpdateConRotParam_Implementation(FRotator InRotator)
{
	CurrentConRot = InRotator;
}
bool ACharacterBase::Server_UpdateConRotParam_Validate(FRotator InRotator)
{
	return true;
}

void ACharacterBase::AddSoul(int32 InSoul)
{
	if(InSoul == 0) return;
	
	Soul + InSoul < 0 ? 0 : Soul += InSoul;

	UpdatedSoul.Broadcast(InSoul, InSoul > 0 ? true : false);
}

FVector ACharacterBase::UpdatedLastLocation()
{
	const FActorSpawnParameters SpawnParameters;
	FTransform SpawnTransform = GetActorTransform();

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorUpVector() * -300.f;
	FHitResult OutHIt;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	//bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::ForDuration, OutHIt, true);
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Start, End, {EObjectTypeQuery::ObjectTypeQuery1}, false, IgnoreActors, EDrawDebugTrace::None, OutHIt, true);
	if (bHit)
	{
		return OutHIt.ImpactPoint;
	}

	return FVector::ZeroVector;
}

bool ACharacterBase::CanExecution()
{
	TSet<UPrimitiveComponent*> OverlapComponents;
	GetOverlappingComponents(OverlapComponents);
	for (UPrimitiveComponent* OverlapComponent : OverlapComponents)
	{
		if (OverlapComponent->ComponentTags.Num() == 0) return false;

		if (UBoxComponent* ExecutionTrigger = Cast<UBoxComponent>(OverlapComponent))
		{
			if (ACharacterBase* ParentObject = Cast<ACharacterBase>(ExecutionTrigger->GetOwner()))
			{
				bool bSuccess = GetCombatComponent()->CanExecution(ParentObject, OverlapComponent->ComponentTags[0]);
				return bSuccess;
			}
		}
	}

	return false;
}

void ACharacterBase::CallExecution_Implementation()
{
	TSet<UPrimitiveComponent*> OverlapComponents;
	GetOverlappingComponents(OverlapComponents);
	for (UPrimitiveComponent* OverlapComponent : OverlapComponents)
	{
		if (OverlapComponent->ComponentTags.Num() == 0) return;

		if (UBoxComponent* ExecutionTrigger = Cast<UBoxComponent>(OverlapComponent))
		{
			if (ACharacterBase* ParentObject = Cast<ACharacterBase>(ExecutionTrigger->GetOwner()))
			{
				this->EndCrouch();
				ParentObject->EndCrouch();
				this->GetCombatComponent()->Server_Attacker(ParentObject, OverlapComponent->ComponentTags[0]);
				ParentObject->GetCombatComponent()->Server_Victim(this, OverlapComponent->ComponentTags[0]);
			}
		}
	}
}

void ACharacterBase::UpdatedStepEvent(FName SocketName, bool bUseEvent, float CueVolume)
{
	const FVector WorldLocation = GetMesh()->GetComponentLocation();
	const FVector BreakVector = WorldLocation + FVector(0.f, 0.f, 98.f);

	const FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);

	const FVector Start = FVector(SocketLocation.X, SocketLocation.Y, BreakVector.Z);
	const FVector End = Start - FVector(0.f, 0.f, 151.f);
	FHitResult OutHit;

	bool bHit = UKismetSystemLibrary::LineTraceSingle(this,
		Start, End,
		TraceTypeQuery1,
		true,
		{ this },
		EDrawDebugTrace::None, OutHit, true);
	if (bHit)
	{

		GetEffectComponent()->FootStep(SocketName, CueVolume);
		if(bUseEvent) MakeNoise(GetVelocity().Size() >= WalkSpeed - 10.0f ? 1.0f : 0.0f, this, OutHit.ImpactPoint, 300.0f, TEXT("Noise_Level1"));
	}
}

void ACharacterBase::UpdatedHitEvent(FVector HitLocation)
{
	MakeNoise(1.0f, this, HitLocation, 1000.0f, TEXT("Noise_Level2"));
}
void ACharacterBase::UpdatedLoudNoiseEvent(USoundBase* CueAsset)
{
	//MakeNoise(1.0f, this, GetActorLocation(), 1000.0f, TEXT("Noise_Level3"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), CueAsset, GetActorLocation());
}

