// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/PlayerCharacter.h"
#include "Classes/CharacterBase.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/EffectComponent.h"
#include "Classes/CombatComponent.h"
#include "Classes/Magic/MasterMagicActor.h"
#include "Classes/PlayerControllerBase.h"
#include "Classes/Interaction/SignPointActor.h"
#include "SaveGame/SaveCharacterState.h"

#include "SoulNetworkProject/PlayerHUD.h"
#include "SoulNetworkProject/InGameModeBase.h"
#include "SoulNetworkProject/public/SoulNetworkProjectGameStateBase.h"
#include "Interface/InteractableInterface.h"
#include "Interface/SaveInterface.h"
#include "Interface/CharacterInterface.h"

#include "SoulNetworkProject/ItemData.h"

#include "Components/SphereComponent.h"

#include "Gameframework/SpringArmComponent.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"

#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "SoulNetworkDebug.h"
#include "Classes/ObjectManager.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


#define FORWARD_SEC 0
#define LEFT_SEC 1
#define RIGHT_SEC 2
#define BACK_SEC 3
#define DODGE_SEC 4

class AObjectManager;

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));														
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	TopViewSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TopViewSpringArm"));
	TopViewSpringArm->SetupAttachment(RootComponent);
	TopCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopCamera"));
	TopCamera->SetupAttachment(TopViewSpringArm);

	SetupBodyComp();
	//SetBodyAsset();

	Cloth = CreateDefaultSubobject<UClothComponent>(TEXT("Cloth"));
	Cloth->SetIsReplicated(true);
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->SetIsReplicated(true);
	Lockon = CreateDefaultSubobject<ULockonSystemComponent>(TEXT("Lockon"));
	Lockon->SetIsReplicated(true);


	GetCapsuleComponent()->bHiddenInGame = true;	// 표시 여부는 soul.DrawDebug 로 런타임 제어
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::BeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::EndOverlap);

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->CrouchedHalfHeight = 50.f;

	L_Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("L_Collision"));
	L_Collision->SetCollisionProfileName("NoCollision");
	L_Collision->SetupAttachment(GetMesh(), "hand_l");
	L_Collision->SetNotifyRigidBodyCollision(false);


	R_Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("R_Collision"));
	R_Collision->SetCollisionProfileName("NoCollision");
	R_Collision->SetupAttachment(GetMesh(), "hand_r");
	R_Collision->SetNotifyRigidBodyCollision(false);

	if (GetCombatComponent() && GetInventoryComponent())
	{
		GetCombatComponent()->OnDeathEventUpdated.AddDynamic(this, &APlayerCharacter::PlayerDeathEvent);
		GetInventoryComponent()->OnEquipSlotUpdated.AddDynamic(this, &APlayerCharacter::UpdatedEquipItem);
	}



	SetReplicates(true);
	SetReplicateMovement(true);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetBodyAsset();

	//if (!GetPlayerState()) return;
	//if (GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	//{
	//	Cast<ICharacterInterface>(GetPlayerState())->SetupState(this);
	//}

	OnRep_Helmet();
	OnRep_Armor();
	OnRep_Pant();
	OnRep_Boots();

	if (!GetPlayerState()) return;
	if (GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		Cast<ICharacterInterface>(GetPlayerState())->SetupStartCharacter(this);
		//Cast<ICharacterInterface>(GetPlayerState())->SetupStartPoint(this);
	
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//UpdateHoldKey(DeltaSeconds);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Purple, FString::Printf(TEXT("Yaw_Degreee: %f"), GetActorRotation().Yaw));
	UpdatedLastInputVector();

	if(GetCombatComponent()->GetOutofControl()) GetCharacterMovement()->StopActiveMovement();

	if((InputX != 0.f || InputY != 0.f)) 
	{
		//if (bUseOrientYaw)
		//{
		//}
		//if(GetLockonComponent() && !GetLockonComponent()->GetLockOn()) 
		//if (GetLockonComponent() && GetLockonComponent()->GetLockOn())
		//{
		//	
		//}
			Server_RunInterpInputVector(InputRot);

	}
	if(!GetCharacterMovement()->IsFalling())
	{
		if (bUseAirAttack)
		{
			Server_AirAttack();
		}
		if (AnimValues.bOutofControl && GetCombatComponent()->IsFallingWithStrongHurt() && GetCombatComponent()->GetFallingHurt())
		{
			GetCombatComponent()->RunHurtMotion();
		}
	}
	else
	{
		if (AnimValues.bOutofControl && GetCombatComponent()->IsStrongHurt() && !GetCombatComponent()->GetFallingHurt())
		{
			GetCombatComponent()->RunHurtMotion();
		}
	}

	//DrawDebugString(GetWorld(), FVector(0, 0, 100), (EnumToString<EFaction>(GetFaction())), this, FColor::White, DeltaSeconds);
	if (SoulDebug::IsNetRoleShown())
	{
		DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumToText(GetLocalRole()), this, FColor::White, DeltaSeconds);
	}

#if !UE_BUILD_SHIPPING
	// 캡슐 콜리전 표시를 soul.DrawDebug 에 맞춰 런타임에 동기화한다.
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		const bool bShowCapsule = SoulDebug::IsDrawEnabled();
		if (Capsule->bHiddenInGame == bShowCapsule)
		{
			Capsule->SetHiddenInGame(!bShowCapsule);
		}
	}
#endif

}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(APlayerCharacter, PlayerHUD);
	//DOREPLIFETIME(APlayerCharacter, ControlBase);

	DOREPLIFETIME(APlayerCharacter, E_Helmet);
	DOREPLIFETIME(APlayerCharacter, E_Armor);
	DOREPLIFETIME(APlayerCharacter, E_Pant);
	DOREPLIFETIME(APlayerCharacter, E_Boots);

	DOREPLIFETIME(APlayerCharacter, E_Expendable);
	DOREPLIFETIME(APlayerCharacter, E_Spell);
}

//Return Role Type//
/* 
*/
FString APlayerCharacter::GetEnumToText(ENetRole NetRole)
{
	switch (NetRole)
	{
		case ROLE_None:
			return "None";
		case ROLE_SimulatedProxy:
			return "SimulatedProxy";
		case ROLE_AutonomousProxy:
			return "AutonomousProxy";
		case ROLE_Authority:
			return "Authority";
		default:
			return "Error";
	}
	//return FString::FromInt((uint8)GetStateComponent()->GetCurrentHealth());
}

void APlayerCharacter::LowAttack()
{
	if(!GetStateComponent()) return;

	if (GetHoldType() == EEquipHand::None)
	{
		if (GetMainWeapon().WeaponData.WeaponType == EWeaponType::Bow)
		{
			Server_UpdatedHandType(EEquipHand::Right);
			return;
		}
	}
	else
	{
		if (GetHoldTypeWeapon().WeaponData.WeaponType == EWeaponType::Bow)
		{
			Server_BowMontage(GetPowerup());
		}
	}
	if (GetHoldTypeWeapon().IsSet())
	{
		if(GetHoldTypeWeapon().WeaponData.WeaponType == EWeaponType::Wand && IsMainSpell())
		{
			if(GetStateComponent()->GetCurrentMana() > 0.0f) Server_E_SpellMontage(Get_E_SpellData());
		}
		else if(GetHoldTypeWeapon().WeaponData.WeaponType == EWeaponType::Strike)
		{
			if(!IsPlayingMontage() && CanExecution()) CallExecution();
			else Server_E_LowAttackMontage();
		}
		else
		{
			Server_E_LowAttackMontage();
		}
	}
	else
	{
		Server_LowAttackMontage();
	}

	//if (HoldType == EEquipHand::Left)
	//{
	//	if(!GetSecondaryWeapon().IsSet()) Server_LowAttackMontage();
	//	else
	//	{
	//		Server_E_LowAttackMontage();
	//	}
	//}
	//else
	//{
	//	if (!GetMainWeapon().IsSet()) Server_LowAttackMontage();
	//	else
	//	{
	//		Server_E_LowAttackMontage();
	//	}
	//}
}

void APlayerCharacter::HeavyAttack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(!GetStateComponent() || !AnimInstance) return;

	Server_HeavyAttackMontage();

}

void APlayerCharacter::AOEAttack()
{
	if (!GetStateComponent()) return;

	if (MainWeapon.IsSet() || SecondaryWeapon.IsSet()) 
	{
		Server_E_AOEAttackMontage();
	}
	else Server_AOEAttackMontage();
}

void APlayerCharacter::WandAttack()
{
	if(!GetStateComponent()) return;

	if (GetStateComponent()->GetCurrentMana() <= 0.f || !E_Spell.IsSet())
	{
		Server_E_LowAttackMontage();
	}
	else
	{
		if (E_Spell.SpellData.SpellType == ESpellType::AttackSorcery || E_Spell.SpellData.SpellType == ESpellType::AssistSorcery)
		{
			Server_E_SpellMontage(E_Spell);
		}
		else
		{
			Server_E_LowAttackMontage();
		}
	}
}

void APlayerCharacter::TalismanAttack()
{
	if(!GetStateComponent()) return;

	if (GetStateComponent()->GetCurrentMana() <= 0.f || !E_Spell.IsSet())
	{
		Server_E_LowAttackMontage();
	}
	else
	{
		if (E_Spell.SpellData.SpellType == ESpellType::AssistIncantation)
		{
			if (E_Spell.ItemData.ItemName.ToString().Contains(TEXT("Enchant")))
			{
				if (MainWeapon.IsSet())
				{
					if(MainWeapon.WeaponData.WeaponType == EWeaponType::Strike) Server_E_SpellMontage(E_Spell);
					else Server_E_LowAttackMontage();
				}
				else
				{
					Server_E_LowAttackMontage();
				}
			}
			else
			{
				Server_E_SpellMontage(E_Spell);
			}
		}
		else if(E_Spell.SpellData.SpellType == ESpellType::AttackIncantation)
		{
			Server_E_SpellMontage(E_Spell);

		}
	}
}



void APlayerCharacter::EndInputChecking()
{
	// ���. //
	if (AnimValues.bLowAttack)
	{
		if (InputType.bButtonWhenAttack)
		{
			if (GetHoldTypeWeapon().IsSet() && GetHoldTypeWeapon().WeaponData.WeaponType == EWeaponType::Bow)
			{
				Server_BowMontage(GetPowerup());
			}
			else
			{
				GetPowerup() ?
					[&]() {EndMt(); HeavyAttack();}() : 
					[&]() { AnimValues.AllTransFalse(); InputType.AllTransFalse(); LowAttack(); }();
			}
		}
		else if (InputType.bButtonWhenDualAttack)
		{
			EndMt();
			Server_DualAttackMontage();
		}
		else if (InputType.bButtonWhenSpell)
		{
			EndMt();
			Server_E_SpellMontage(E_Spell);
		}
		else if (InputType.bButtonWhenAOE)
		{
			EndMt();
			AOEAttack();
		}
		else if (InputType.bButtonWhenRoll)
		{
			EndMt();
			InputValueRollNDodge();
		}
		else if (InputType.bButtonWhenExpendable)
		{
			EndMt();
			Server_UseExpendable(GetExpendableData());
		}
	}
	// ����. //
	else if (AnimValues.bSpellAttack)
	{
		if (InputType.bButtonWhenAttack)
		{
			GetPowerup() ?
				[&]() {EndMt(); HeavyAttack(); }() :
				[&]() {EndMt(); LowAttack(); }();
		}
		else if (InputType.bButtonWhenDualAttack)
		{
			EndMt();
			Server_DualAttackMontage();
		}
		else if (InputType.bButtonWhenSpell)
		{
			SpellData == E_Spell ? 
			[&]() { AnimValues.AllTransFalse(); InputType.AllTransFalse(); }() :
			[&]() { EndMt(); }();
			
			WandAttack();
		}
		else if (InputType.bButtonWhenAOE)
		{
			EndMt();
			AOEAttack();
		}
		else if (InputType.bButtonWhenRoll)
		{

			EndMt();
			InputValueRollNDodge();
		}
		else if (InputType.bButtonWhenExpendable)
		{
			EndMt();
			Server_UseExpendable(GetExpendableData());
		}
	}
	// AOE. //
	else if (AnimValues.bAOEAttack)
	{
		if (InputType.bButtonWhenAttack)
		{
			GetPowerup() ?
				[&]() {EndMt(); HeavyAttack(); }() :
				[&]() {EndMt(); LowAttack(); }();
		}
		else if (InputType.bButtonWhenDualAttack)
		{
			EndMt();
			Server_DualAttackMontage();
		}
		else if (InputType.bButtonWhenSpell)
		{
			EndMt();

			Server_E_SpellMontage(E_Spell);
		}
		else if (InputType.bButtonWhenAOE)
		{
			AnimValues.AllTransFalse();
			InputType.AllTransFalse();

			AOEAttack();
		}
		else if (InputType.bButtonWhenRoll)
		{
			EndMt();
			InputValueRollNDodge();
		}
		else if (InputType.bButtonWhenExpendable)
		{
			EndMt();
			Server_UseExpendable(GetExpendableData());
		}
	}
	// ���� ����. //
	else if (AnimValues.bDualAttack)
	{
		if (InputType.bButtonWhenAttack)
		{
			EndMt();
			LowAttack();
			//if (PressedKey == EPressedKey::Sh_BT) HeavyAttack();
			//else LowAttack();
		}
		else if (InputType.bButtonWhenDualAttack)
		{
			InputType.AllTransFalse();
			AnimValues.AllTransFalse();

			Server_DualAttackMontage();
		}
		else if (InputType.bButtonWhenSpell)
		{
			EndMt();

			Server_E_SpellMontage(E_Spell);
		}
		else if (InputType.bButtonWhenAOE)
		{
			EndMt();
			AOEAttack();
		}
		else if (InputType.bButtonWhenRoll)
		{
			EndMt();
			InputValueRollNDodge();
		}
		else if (InputType.bButtonWhenExpendable)
		{
			EndMt();
			Server_UseExpendable(GetExpendableData());
		}
	}
	// ����. //
	else if (AnimValues.bOutofControl)
	{
		if (InputType.bButtonWhenRoll)
		{
			EndMt();
			InputValueRollNDodge();
		}
	}
	else
	{
		if (InputType.bButtonWhenAttack)
		{
			EndMt();

			//if (PressedKey == EPressedKey::Sh_BT) HeavyAttack();
			//else LowAttack();
		}
		else if (InputType.bButtonWhenDualAttack)
		{
			EndMt();
			Server_DualAttackMontage();
		}
		else if (InputType.bButtonWhenSpell)
		{
			EndMt();

			Server_E_SpellMontage(E_Spell);
		}
		else if (InputType.bButtonWhenAOE)
		{
			EndMt();
			AOEAttack();
		}
		else if (InputType.bButtonWhenRoll)
		{
			EndMt();
			InputValueRollNDodge();
		}
		else if (InputType.bButtonWhenExpendable)
		{
			EndMt();
			Server_UseExpendable(GetExpendableData());
		}
	}
	
	//GetCombatComponent()->SetFalseHitSurface();
}

void APlayerCharacter::Server_ToggleSpeed_Implementation(float InSpeed)
{
	Multicast_ToggleSpeed(InSpeed);
}
bool APlayerCharacter::Server_ToggleSpeed_Validate(float InSpeed)
{
	return true;
}
void APlayerCharacter::Multicast_ToggleSpeed_Implementation(float InSpeed)
{
	ToggleSpeed(InSpeed);
}
bool APlayerCharacter::Multicast_ToggleSpeed_Validate(float InSpeed)
{
	return true;
}

void APlayerCharacter::SetInputValue(float AxisValue, bool bForward)
{
	/*
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(FRotator(0.f, GetControlRotation().Yaw, 0.f)); 
	FVector RightVector = UKismetMathLibrary::GetRightVector(FRotator(0.f, GetControlRotation().Yaw, 0.f));

	FVector InputDirection = (RightVector * AxisX) + (ForwardVector * AxisY);
	//InverseTransformDirection : ���忡�� �������� ���ð����� ��ȯ ������ = ���忡���� �޴°��� ��� �޶������� ���ÿ��� Direction(����)�� ���� ���� �Ȱ���.//
	FVector TransitionLocalDirectionVector = UKismetMathLibrary::InverseTransformDirection(GetActorTransform(), InputDirection);

	//InputX = TransitionLocalDirectionVector.Y;
	//InputY = TransitionLocalDirectionVector.X;
	*/

	bForward ? InputY = AxisValue : InputX = AxisValue;
	FVector Acceleration = FVector::ZeroVector;
	if (!bForward)
	{
		if (AxisValue == 0.0f) Acceleration = FVector::ZeroVector;
		else if (AxisValue != 0.0f)
		{

			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			Acceleration = Direction * AxisValue;
		}
	}
	else if(bForward)
	{
		if (AxisValue == 0.0f) Acceleration = FVector::ZeroVector;
		else if (AxisValue != 0.0f)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			Acceleration = Direction * AxisValue;
		}
	}
	InputConVector += Acceleration;

	if (GetLockonComponent() && GetLockonComponent()->GetLockChr())
	{
		InputRot = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(), 
			GetLockonComponent()->GetLockChr()->GetActorLocation());
	}
	else
	{

		if (InputX != 0.f || InputY != 0.f)
		{
			InputRot = FRotator(0.f, UKismetMathLibrary::Conv_VectorToRotator(LastInputConVector).Yaw, 0.f);
		}
	}
}
void APlayerCharacter::UpdatedLastInputVector()
{
	LastInputConVector = InputConVector;
	InputConVector = FVector::ZeroVector;
}


void APlayerCharacter::AttachEquipItem(FInventoryData AttachData, FName SocketName)
{
	switch (AttachData.ItemData.ItemType)
	{
		case EItemType::All:
			return;

		case EItemType::Weapon:
		{
			Server_AttachItem(AttachData);
		}
		break;

		case EItemType::Helmet:
		{
			Server_PermuteData(EItemType::Helmet, AttachData);
			//BodySlot(E_Helmet.ItemData, E_Helmet.ClothData);
		}
		break;

		case EItemType::Armor:
		{
			Server_PermuteData(EItemType::Armor, AttachData);
			//BodySlot(E_Armor.ItemData, E_Armor.ClothData);
		}
		break;

		case EItemType::Pant:
		{
			Server_PermuteData(EItemType::Pant, AttachData);
			//BodySlot(E_Pant.ItemData, E_Pant.ClothData);
		}
		break;

		case EItemType::Boots:
		{
			Server_PermuteData(EItemType::Boots, AttachData);
			//BodySlot(E_Boots.ItemData, E_Boots.ClothData);
		}
		break;

		case EItemType::Expendable:
		{
			Server_PermuteData(EItemType::Expendable, AttachData);
		}
		break;

		case EItemType::Arrow:
		{
			Server_PermuteData(EItemType::Arrow, AttachData);
		}
		break;

		case EItemType::Spell:
		{
			Server_PermuteData(EItemType::Spell, AttachData);
		}
		break;

		default:
		return;
	}
}

void APlayerCharacter::RemoveEquipItem(EItemType ItemType, EEquipHand HandType)
{

	switch (ItemType)
	{
	case EItemType::All:
		return;
	
	case EItemType::Weapon:
	{
		Server_DetachItem(ItemType, HandType);
	}
	break;
	case EItemType::Helmet:
	{
		if (E_Helmet.IsSet())
		{
			//Cloth->Server_RemoveBodySlot(E_Helmet.ItemData, E_Helmet.ClothData);
			//Server_ResetData(EItemType::Helmet);
		}
	}
	break;
	
	case EItemType::Armor:
	{
		if (E_Armor.IsSet())
		{
			//Cloth->Server_RemoveBodySlot(E_Armor.ItemData, E_Armor.ClothData);
			//Server_ResetData(EItemType::Armor);
		}
	}
	break;
	
	case EItemType::Pant:
	{
		if (E_Pant.IsSet())
		{
			//Cloth->Server_RemoveBodySlot(E_Pant.ItemData, E_Pant.ClothData);
			//Server_ResetData(EItemType::Pant);
		}
	}
	break;
	
	case EItemType::Boots:
	{
		if (E_Boots.IsSet())
		{
			//Cloth->Server_RemoveBodySlot(E_Boots.ItemData, E_Boots.ClothData);
			//Server_ResetData(EItemType::Boots);
		}
	}
	break;
	
	case EItemType::Expendable:
	{
		if (E_Expendable.IsSet())
		{
			Server_ResetData(EItemType::Expendable);
		}
	}
	break;
	case EItemType::Arrow:
	{
		if (E_FirstArrow.IsSet())
		{
			Server_ResetData(EItemType::Arrow);
		}
	}

	case EItemType::Spell:
	{
		if (E_Spell.IsSet())
		{
			Server_ResetData(EItemType::Spell);
		}
	}
	break;
	default:
		return;
	}
}

bool APlayerCharacter::GetCharacterMoving()
{
	bool bMoving = false;
	if(GetCharacterMovement()->IsFalling() || AnimValues.ValueCheckBoolean()) bMoving = true;

	return bMoving;
}

void APlayerCharacter::StartMotion()
{
	Server_StartMotion();
}

void APlayerCharacter::Server_StartMotion_Implementation()
{
	Multicast_StartMotion();
}

bool APlayerCharacter::Server_StartMotion_Validate()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !StartStandAnimMontage) return false;

	return true;
}

void APlayerCharacter::Multicast_StartMotion_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !StartStandAnimMontage) return;


	AnimValues.bMtMotion = true;
	bStartingStand = true;
	//if (!AnimInstance->IsAnyMontagePlaying())
	//{
	//}
		AnimInstance->Montage_Play(StartStandAnimMontage);
	
}

bool APlayerCharacter::Multicast_StartMotion_Validate()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !StartStandAnimMontage) return false;

	return true;
}

void APlayerCharacter::Server_AirAttack_Implementation()
{
	Multicast_AirAttack();
}
bool APlayerCharacter::Server_AirAttack_Validate()
{
	return true;
}


void APlayerCharacter::Multicast_AirAttack_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(!AnimInstance || !AirAttackMt) return;

	AnimValues.bAirAttack = true;

	const char* SectionList[] = { "Start", "Loop", "Landing"};
	FString SectionName;
	SectionName = HoldType == EEquipHand::None ? "1H_" : "2H_";
	SectionName += bUseAirAttack ? "Landing" : "Start";


	if (!AnimInstance->Montage_IsPlaying(AirAttackMt))
	{
		AnimInstance->Montage_Play(AirAttackMt);
		AnimInstance->Montage_JumpToSection(FName(SectionName));

		bUseAirAttack = true;
	}
	else if (AnimInstance->Montage_IsPlaying(AirAttackMt))
	{
		AnimInstance->Montage_Play(AirAttackMt);
		AnimInstance->Montage_JumpToSection(FName(SectionName));

		bUseAirAttack = false;
		//GetCharacterMovement()->StopActiveMovement();
	}
}
bool APlayerCharacter::Multicast_AirAttack_Validate()
{
	return true;
}

void APlayerCharacter::UpdatePointDamage(AActor* DamageCauser)
{
	Client_PointEvent();
	//if(!ControlBase) return;
	//
	//ControlBase->LowShakeCamera();
}

void APlayerCharacter::UpdateRadialDamage(AActor* DamageCauser)
{
}

void APlayerCharacter::UpdatedExecutionDamage(AActor* DamageCauser)
{
}

void APlayerCharacter::UpdatedHitSurface(FHitResult HitResult, EAttackType AttackType)
{

	GetEffectComponent()->EffectSequence(EEffectType::Surface, HitResult.ImpactPoint, HitResult);
	UpdatedHitEvent(GetActorLocation());

	if (GetCombatComponent())
	{
		GetCombatComponent()->UpdatedCameraShakeArea(HitResult.ImpactPoint, GetCombatComponent()->GetCameraShakeRadiusType(AttackType), AttackType);
	}
	//TArray<AActor*> OutActors;
	//UKismetSystemLibrary::SphereOverlapActors(GetWorld(), HitResult.ImpactPoint, 100.f, { UEngineTypes::ConvertToObjectType(ECC_Pawn) }, APlayerCharacter::StaticClass(), { CO }, OutActors);
	//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 100.f, 12, FColor::Red, true, 10.0f);
	//RunCameraShakeArea(OutActors);
}

void APlayerCharacter::UpdatedSpell(float Value)
{
	if (GetStateComponent() && GetSpellActor() && AnimValues.bSpellAttack && GetLocalRole() == ROLE_Authority)
	{
		GetStateComponent()->AddMana(-Value);
		if(GetStateComponent()->GetCurrentMana() <= 0.0f && GetCharging()) Server_E_SpellMontage(GetSpellData());

		//GetSpellActor()->SpellData.bDetachAtLoop ? Server_ClearSpellActor() : [&]{} ;
	}
}

void APlayerCharacter::UpdatedDeathFunc()
{
	if (GetLockonComponent() && 
		GetLockonComponent()->GetLockChr() && 
		GetLockonComponent()->GetLockChr()->GetStateComponent()->GetCurrentHealth() <= 0.0f)
	{
		GetLockonComponent()->SetFocusActor(nullptr);
	}
	//GetAimComponent()->SetFocusActor(nullptr);

	if (GetStateComponent()->GetCurrentHealth() <= 0)
	{
		GetLockonComponent()->SetFocusActor(nullptr);

		if (!AnimValues.bOutofControl)
		{
			GetCombatComponent()->Server_RunDeadMotion();
		}
		bDead = true;
	}
}

void APlayerCharacter::InputValueRollNDodge()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	
	if (InputX == 0.f && InputY == 0.f)
	{
		Server_RollMontage(DODGE_SEC);
	}
	else
	{
		//bUseControllerRotationYaw = false;
		Server_SetUseControllerYaw(false);
		FRotator NewRot = UKismetMathLibrary::Conv_VectorToRotator(LastInputConVector);

		Server_RollMontage(FORWARD_SEC);
		Server_SetRotation(NewRot);


		//float Degree = FMath::Atan2(InputX, InputY);
		//float Yaw_Degreee = Degree * 180 / PI;
		//FRotator NewRot = FRotator(0.0f, GetActorRotation().Yaw + Yaw_Degreee, 0.0f);
		//if (!bUseControllerRotationYaw)
		//{
		//	FRotator NewRot = UKismetMathLibrary::Conv_VectorToRotator(GetLastMovementInputVector());
		//	Server_RollMontage(FORWARD_SEC);
		//	Server_SetRotation(NewRot);
		//}
		//else if (bUseControllerRotationYaw)
		//{
		//	float Degree = UKismetMathLibrary::Atan2(InputX, InputY);
		//	float Yaw_Degreee = Degree * 180 / PI;
		//	//UE_LOG(LogTemp, Warning, TEXT("Yaw_Degress : %1f"), Yaw_Degreee);
		//
		//	FRotator NewRot = UKismetMathLibrary::Conv_VectorToRotator(GetLastMovementInputVector());
		//	if (Yaw_Degreee >= -45.f && Yaw_Degreee <= 45.f)
		//	{
		//		Server_RollMontage(FORWARD_SEC);
		//		//UE_LOG(LogTemp, Warning, TEXT("Forward"));
		//	}
		//	else if (Yaw_Degreee < -45.f && Yaw_Degreee >= -130.f)
		//	{
		//		Server_RollMontage(LEFT_SEC);
		//		//UE_LOG(LogTemp, Warning, TEXT("Left"));
		//	}
		//	else if (Yaw_Degreee > 45.f && Yaw_Degreee <= 130.f)
		//	{
		//		Server_RollMontage(RIGHT_SEC);
		//		//UE_LOG(LogTemp, Warning, TEXT("Right"));
		//	}
		//	else if ((Yaw_Degreee < -130.f && Yaw_Degreee >= -180.f) || (Yaw_Degreee > 130.f && Yaw_Degreee <= 180.f))
		//	{
		//		Server_RollMontage(BACK_SEC);
		//		//UE_LOG(LogTemp, Warning, TEXT("Back_Left"));
		//	}
		//	SetActorRotation(NewRot);
		//
		//}
	}
	//State->AddStamina(-10.f);
}

FRotator APlayerCharacter::GetFocusRotator() const
{
	return  FRotator(GetControlRotation().Pitch + 10.f, GetActorRotation().Yaw, GetControlRotation().Roll);
}

void APlayerCharacter::BindProperty(APlayerControllerBase* Con, APlayerHUD* HUD)
{
	ControlBase = Con;
	PlayerHUD = HUD;
}

void APlayerCharacter::ClearPopupWidget()
{
	TArray<AActor*> OverlapActors;
	GetOverlappingActors(OverlapActors);
	if (OverlapActors.Num() > 0)
	{
		for (auto& OverlapActor : OverlapActors)
		{
			if (OverlapActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			Cast<IInteractableInterface>(OverlapActor)->UpdatedOverlapEnd();
		}
	}
}

void APlayerCharacter::Client_PopupMenu_Implementation(AActor* ParentActor, TSubclassOf<class UUserWidget> PopupClass, bool bGamePaused)
{
	if (PlayerHUD && ControlBase)
	{
		ControlBase->SetPause(bGamePaused);
		PlayerHUD->ShowPopupMenu(ParentActor, PopupClass);
	}
}

void APlayerCharacter::Server_RefreshManagers_Implementation()
{
	AObjectManager* OutActor = Cast<AObjectManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AObjectManager::StaticClass()));
	if (OutActor)
	{
		OutActor->RefreshManagers();
	}
}

void APlayerCharacter::Server_SetUseControllerYaw_Implementation(bool bActive)
{
	bUseControllerRotationYaw = bActive;
}

void APlayerCharacter::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if (OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	//{
	//	TArray<AActor*> OverlapActors;
	//	GetOverlappingActors(OverlapActors);
	//
	//	if (ASignPointActor* OverlapActor = Cast<ASignPointActor>(OtherActor))
	//	{
	//		Client_OverlapInteraction(OverlapActor->Collision);
	//	}
	//
	//	if(OverlapActors.Num() == 1)
	//	{
	//		InteractionIndex = 0;
	//		Server_OverlapActors(true, GetInteractionText(OverlapActors[InteractionIndex]));
	//	}
	//}
	Client_OverlapInteraction(OtherActor);

}

void APlayerCharacter::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if (OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	//{
	//	Cast<IInteractableInterface>(OtherActor)->UpdatedOverlapEnd();
	//
	//	TArray<AActor*> OverlapActors;
	//	GetOverlappingActors(OverlapActors);
	//
	//	if (OverlapActors.Num() > 0)
	//	{
	//		for (int8 i = 0; i < OverlapActors.Num(); i++)
	//		{
	//			if (IsInteractionActor(OverlapActors[i]))
	//			{
	//				Server_OverlapActors(true, GetInteractionText(OverlapActors[i]));
	//				return;
	//			}
	//		}
	//		Server_OverlapActors(false);
	//	}
	//	else if (OverlapActors.Num() == 0)
	//	{
	//		Server_OverlapActors(false);
	//	}
	//}

	//Client_OverlapInteraction(OtherActor);
	Client_EndOverlapInteraction(OtherActor);
}

void APlayerCharacter::UpdatedEquipItem(EItemType SlotType, EEquipHand HandType, FInventoryData ItemData, bool bAnim)
{
	switch (SlotType)
	{
	case EItemType::All:
		return;

	case EItemType::Weapon:
	{
		ItemData.IsSet() ? Server_AttachItem(ItemData) : Server_DetachItem(EItemType::Weapon, HandType);
	}
	break;

	case EItemType::Helmet:
	{
		ItemData.IsSet() ? 
			[&]{ Server_PermuteData(SlotType, ItemData); GetClothComponent()->Server_AddBodySlot(E_Helmet.ItemData, E_Helmet.ClothData);}() :
			[&] {if (E_Helmet.IsSet()) { GetClothComponent()->Server_RemoveBodySlot(E_Helmet.ItemData, E_Helmet.ClothData); Server_ResetData(EItemType::Helmet);}}();
	}
	break;

	case EItemType::Armor:
	{
		ItemData.IsSet() ?
			[&] { Server_PermuteData(SlotType, ItemData); GetClothComponent()->Server_AddBodySlot(E_Armor.ItemData, E_Armor.ClothData); }() :
			[&] {if (E_Armor.IsSet()) { GetClothComponent()->Server_RemoveBodySlot(E_Armor.ItemData, E_Armor.ClothData); Server_ResetData(EItemType::Armor); }}();
	}
	break;

	case EItemType::Pant:
	{
		ItemData.IsSet() ?
			[&] { Server_PermuteData(SlotType, ItemData); GetClothComponent()->Server_AddBodySlot(E_Pant.ItemData, E_Pant.ClothData); }() :
			[&] {if (E_Pant.IsSet()) { GetClothComponent()->Server_RemoveBodySlot(E_Pant.ItemData, E_Pant.ClothData); Server_ResetData(EItemType::Pant); }}();
	}
	break;

	case EItemType::Boots:
	{
		ItemData.IsSet() ?
			[&] { Server_PermuteData(SlotType, ItemData); GetClothComponent()->Server_AddBodySlot(E_Boots.ItemData, E_Boots.ClothData); }() :
			[&] {if (E_Boots.IsSet()) { GetClothComponent()->Server_RemoveBodySlot(E_Boots.ItemData, E_Boots.ClothData); Server_ResetData(EItemType::Boots); }}();
	}
	break;

	case EItemType::Expendable:
	{
		ItemData.IsSet() ?
			Server_PermuteData(EItemType::Expendable, ItemData) :
			Server_ResetData(EItemType::Expendable);
	}
	break;

	case EItemType::Arrow:
	{
		ItemData.IsSet() ?
			Server_AttachItem(ItemData) :
			[this]()
			{ 
				if(!this->Inventory->ArrowEquip.Find(0)) this->Server_DetachItem(EItemType::Arrow, EEquipHand::None, 0);
				else if(!this->Inventory->ArrowEquip.Find(1)) this->Server_DetachItem(EItemType::Arrow, EEquipHand::None, 1);
			};
	}
	break;

	case EItemType::Spell:
	{
		ItemData.IsSet() ?
			Server_PermuteData(EItemType::Spell, ItemData) :
			Server_ResetData(EItemType::Spell);
	}
	break;

	default:
		return;
	}
}

bool APlayerCharacter::IsInteractionActor(AActor* InteractionActor)
{
	if (InteractionActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass())) return true;
	return false;
}

float APlayerCharacter::GetApplyDamge(EEquipHand HandType) const
{
	float ApplyDamageValue = 3.f;
/*
*/
	if (HandType == EEquipHand::Left)
	{
		if (HoldType == EEquipHand::None)
		{
			if (GetSecondaryWeapon().IsSet())
			{
				ApplyDamageValue += GetSecondaryWeapon().WeaponData.DamageStat[GetSecondaryWeapon().ItemCount].STRStat;
			}
		}
	}
	if (HandType == EEquipHand::Right)
	{
		if (HoldType != EEquipHand::Left)
		{
			if (GetMainWeapon().IsSet())
			{
				ApplyDamageValue += GetMainWeapon().WeaponData.DamageStat[GetMainWeapon().ItemCount].STRStat;
			}
		}
		else
		{
			if (GetSecondaryWeapon().IsSet())
			{
				ApplyDamageValue += GetSecondaryWeapon().WeaponData.DamageStat[GetSecondaryWeapon().ItemCount].STRStat;
			}
		}
	}


	return ApplyDamageValue;
}

FInventoryData APlayerCharacter::Get_E_ClothData(EItemType ItemType) const
{
	switch (ItemType)
	{
		case EItemType::Helmet :
		return E_Helmet;
		case EItemType::Armor :
		return E_Armor;
		case EItemType::Pant :
		return E_Pant;
		case EItemType::Boots : 
		return E_Boots;
		default:
			break;
	}
	return FInventoryData{};
}

FText APlayerCharacter::GetInteractionText(AActor* InteractionActor)
{
	FName Description = NAME_None;

	if (InteractionActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		Description = Cast<IInteractableInterface>(InteractionActor)->GetInteractableName();
	}

	return FText::FromName(Description);
}

void APlayerCharacter::SetupBodyComp()
{
	//Weapon Mesh//
	

	//Body Mesh//
	Body_Chest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_Chest"));
	Body_Chest->SetupAttachment(GetMesh());
	Body_Chest->ComponentTags.Add(TEXT("BODY_CHEST"));
	Body_Chest->SetIsReplicated(true);

	Body_Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_Arms"));
	Body_Arms->SetupAttachment(GetMesh());
	Body_Arms->ComponentTags.Add(TEXT("BODY_ARMS"));
	Body_Arms->SetIsReplicated(true);

	Body_hands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_hands"));
	Body_hands->SetupAttachment(GetMesh());
	Body_hands->ComponentTags.Add(TEXT("BODY_HANDS"));
	Body_hands->SetIsReplicated(true);

	Body_Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_Legs"));
	Body_Legs->SetupAttachment(GetMesh());
	Body_Legs->ComponentTags.Add(TEXT("BODY_LEDGS"));
	Body_Legs->SetIsReplicated(true);

	Body_Feets = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body_Feets"));
	Body_Feets->SetupAttachment(GetMesh());
	Body_Feets->ComponentTags.Add(TEXT("BODY_FEETS"));
	Body_Feets->SetIsReplicated(true);
	//////////

	//EquipMesh//
	Eq_Helmet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eq_Helmet"));
	Eq_Helmet->SetupAttachment(GetMesh());
	Eq_Helmet->ComponentTags.Add(TEXT("HELMET"));
	Eq_Helmet->SetIsReplicated(true);

	Eq_Armor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eq_Armor"));
	Eq_Armor->SetupAttachment(GetMesh());
	Eq_Armor->ComponentTags.Add(TEXT("ARMOR"));
	Eq_Armor->SetIsReplicated(true);

	Eq_Pant = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eq_Pant"));
	Eq_Pant->SetupAttachment(GetMesh());
	Eq_Pant->ComponentTags.Add(TEXT("PANT"));
	Eq_Pant->SetIsReplicated(true);

	Eq_Boots = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Eq_Boots"));
	Eq_Boots->SetupAttachment(GetMesh());
	Eq_Boots->ComponentTags.Add(TEXT("BOOTS"));
	Eq_Boots->SetIsReplicated(true);
	/////////////
}

//�ش�޽��� �⺻�޽��� ��¡��Ŵ//
void APlayerCharacter::SetBodyAsset()
{
	Body_Chest->SetMasterPoseComponent(GetMesh());
	Body_Arms->SetMasterPoseComponent(GetMesh());
	Body_hands->SetMasterPoseComponent(GetMesh());
	Body_Legs->SetMasterPoseComponent(GetMesh());
	Body_Feets->SetMasterPoseComponent(GetMesh());

	Eq_Helmet->SetMasterPoseComponent(GetMesh());
	Eq_Armor->SetMasterPoseComponent(GetMesh());
	Eq_Pant->SetMasterPoseComponent(GetMesh());
	Eq_Boots->SetMasterPoseComponent(GetMesh());
}
///////////////////////////////////


//������ ����ϱ�//
void APlayerCharacter::Server_UseExpendable_Implementation(FInventoryData UseExpendableData)
{
	Multicast_UseExpendable(UseExpendableData);
}
bool APlayerCharacter::Server_UseExpendable_Validate(FInventoryData UseExpendableData)
{
	return true;
}

void APlayerCharacter::Multicast_UseExpendable_Implementation(FInventoryData UseExpendableData)
{
	if(IsPlayingMontage() || !UseExpendableData.IsSet() ||UseExpendableData.ItemData.ItemType != EItemType::Expendable) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(!AnimInstance || !UseExpendableData.ExpendableData.ReactionMt) return;

	//FName SectionName = FName(GetSectionType(IncreaseData));
	AnimValues.bExpendable = true;

	FString SectionName;
	SectionName = "Type_1";

	IncreaseData.ResetValue();
	IncreaseData.PermuteValue(UseExpendableData);

	
	AnimInstance->Montage_Play(UseExpendableData.ExpendableData.ReactionMt);
	AnimInstance->Montage_JumpToSection(FName(SectionName), UseExpendableData.ExpendableData.ReactionMt);
}
bool APlayerCharacter::Multicast_UseExpendable_Validate(FInventoryData UseExpendableData)
{
	return true;
}
///////////////////


void APlayerCharacter::Client_BindBossActor_Implementation(ABossCharacter* BindActor)
{
	TargetBoss = BindActor;
}

void APlayerCharacter::Server_PermuteData_Implementation(EItemType ApplyItemType, FInventoryData PermuteData)
{
	switch (ApplyItemType)
	{
		case EItemType::All:
		case EItemType::Weapon:
		return;

		case EItemType::Helmet:
		{
			E_Helmet.PermuteValue(PermuteData);
		}
		break;
		case EItemType::Armor:
		{
			E_Armor.PermuteValue(PermuteData);
		}
		break;
		case EItemType::Pant:
		{
			E_Pant.PermuteValue(PermuteData);
		}
		break;
		case EItemType::Boots:
		{
			E_Boots.PermuteValue(PermuteData);
		}
		break;
		case EItemType::Expendable:
		{
			E_Expendable.PermuteValue(PermuteData);
		}
		break;
		case EItemType::Arrow:
		{
			E_FirstArrow.PermuteValue(PermuteData);
		}
		break;
		case EItemType::Spell:
		{
			E_Spell.PermuteValue(PermuteData);
		}
		break;

		default:
		return;
	}
	
}
bool APlayerCharacter::Server_PermuteData_Validate(EItemType ApplyItemType, FInventoryData PermuteData)
{
	return true;
}

void APlayerCharacter::Server_ResetData_Implementation(EItemType ItemType)
{
	switch (ItemType)
	{
		case EItemType::All:
		break;
		case EItemType::Weapon:
		break;
		case EItemType::Helmet:
		{
			E_Helmet.ResetValue();
		}
		break;
		case EItemType::Armor:
		{
			E_Armor.ResetValue();
		}
		break;
		case EItemType::Pant:
		{
			E_Pant.ResetValue();
		}
		break;
		case EItemType::Boots:
		{
			E_Boots.ResetValue();
		}
		break;
		case EItemType::Expendable:
		{
			E_Expendable.ResetValue();
		}
		break;
		case EItemType::Arrow:
		{
			E_FirstArrow.ResetValue();
		}
		break;
		case EItemType::Spell:
		{
			E_Spell.ResetValue();
		}
		break;
		default:
			break;
	}
}
bool APlayerCharacter::Server_ResetData_Validate(EItemType ItemType)
{
	return true;
}

void APlayerCharacter::Server_OverlapActors_Implementation(bool bVisible, const FText& InText)
{
	Client_OverlapActors(bVisible, InText);
}
bool APlayerCharacter::Server_OverlapActors_Validate(bool bVisible, const FText& InText)
{
	return true;
}

void APlayerCharacter::Client_OverlapActors_Implementation(bool bVisible, const FText& InText)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetupInteractionBox(bVisible, InText);
		bActiveInteraction = bVisible ? true : false;
	}
}
bool APlayerCharacter::Client_OverlapActors_Validate(bool bVisible, const FText& InText)
{
	return true;
}

void APlayerCharacter::MeleeTrace(EEquipHand HandType)
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);


	FHitResult HitResult;
	FVector Start;
	FVector End;
	float RadiusSize = 0.f;
	FRotator Orientation;
	if (HandType == EEquipHand::Left)
	{
		Start = L_Collision->GetComponentLocation();
		End = L_Collision->GetComponentLocation();
		RadiusSize = L_Collision->GetScaledBoxExtent().Size();
		Orientation = L_Collision->GetRelativeRotation();
	}
	else if (HandType == EEquipHand::Right)
	{
		Start = R_Collision->GetComponentLocation();
		End = R_Collision->GetComponentLocation();
		RadiusSize = R_Collision->GetScaledBoxExtent().Size();
		Orientation = R_Collision->GetRelativeRotation();
	}
	else if (HandType == EEquipHand::None)
	{
		if(HoldType == EEquipHand::None) return;
		else if (HoldType == EEquipHand::Left)
		{
			Start = SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Shield ?
				GetMainShieldMesh()->GetSocketLocation("Trace_Start") : 
				GetMainWeaponMesh()->GetSocketLocation("Trace_Start");

			End = SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Shield ?
				GetMainShieldMesh()->GetSocketLocation("Trace_End") :
				GetMainWeaponMesh()->GetSocketLocation("Trace_End");

			RadiusSize = SecondaryWeapon.WeaponData.TraceSize;
		}
		else if (HoldType == EEquipHand::Right)
		{
			Start = MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ?
				GetMainShieldMesh()->GetSocketLocation("Trace_Start") :
				GetMainWeaponMesh()->GetSocketLocation("Trace_Start");

			End = MainWeapon.WeaponData.WeaponType == EWeaponType::Shield ?
				GetMainShieldMesh()->GetSocketLocation("Trace_End") :
				GetMainWeaponMesh()->GetSocketLocation("Trace_End");

			RadiusSize = SecondaryWeapon.WeaponData.TraceSize;
		}
	}


	bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, RadiusSize, TraceTypeQuery3, false, IgnoreActors, SoulDebug::Trace(), HitResult, false);
	if (bHit)
	{
		if (ACharacterBase* Chr = Cast<ACharacterBase>(HitResult.GetActor()))// && Chr->GetFaction() != this->GetFaction())
		{
			if (GetFaction() != Chr->GetFaction())
			{
				UGameplayStatics::ApplyPointDamage(Chr, -10.f, HitResult.ImpactPoint, HitResult, this->GetController(), this, GetCombatComponent()->GetDamageType(EAttackType::Low));

			}
		}
	}
}

void APlayerCharacter::OnRep_Helmet()
{
	if (!Cloth) return;
	if (E_Helmet.IsSet())
	{
		GetClothComponent()->Server_AddBodySlot(E_Helmet.ItemData, E_Helmet.ClothData);
	}
}
void APlayerCharacter::OnRep_Armor()
{
	if (!Cloth) return;
	if (E_Armor.IsSet())
	{
		GetClothComponent()->Server_AddBodySlot(E_Armor.ItemData, E_Armor.ClothData);
	}
}
void APlayerCharacter::OnRep_Pant()
{
	if (!Cloth) return;
	if (E_Pant.IsSet())
	{
		GetClothComponent()->Server_AddBodySlot(E_Pant.ItemData, E_Pant.ClothData);
	}
}
void APlayerCharacter::OnRep_Boots()
{
	if (!Cloth) return;
	if (E_Boots.IsSet())
	{
		GetClothComponent()->Server_AddBodySlot(E_Boots.ItemData, E_Boots.ClothData);
	}
}
void APlayerCharacter::OnRep_Expendable()
{
	
}

void APlayerCharacter::SetFocusActor()
{
	if (GetLockonComponent())
	{
		FocusActor = Lockon->GetLockChr();
		FocusBoneName = Lockon->GetLockBoneName();
	}
}

void APlayerCharacter::PlayerDeathEvent()
{
	if (ControlBase)
	{
		ControlBase->PlayFromEndDeathEvent();
	}
}

bool APlayerCharacter::IsMainSpell()
{
	if (MainWeapon.IsSet()
		&& MainWeapon.WeaponData.WeaponType == EWeaponType::Wand
		&& (E_Spell.SpellData.SpellType == ESpellType::AttackSorcery
			|| E_Spell.SpellData.SpellType == ESpellType::AssistSorcery))
	return true;

	return false;
}

bool APlayerCharacter::IsSecondarySpell()
{
	if (SecondaryWeapon.IsSet()
		&& SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Talisman
		&& (E_Spell.SpellData.SpellType == ESpellType::AttackIncantation
			|| E_Spell.SpellData.SpellType == ESpellType::AssistIncantation))
	return true;

	return false;
}

void APlayerCharacter::Server_SetRotation_Implementation(FRotator NewRot)
{
	Multicast_SetRotation(NewRot);
}
void APlayerCharacter::Multicast_SetRotation_Implementation(FRotator NewRot)
{
	SetActorRotation(NewRot);
}

void APlayerCharacter::Server_SetLocation_Implementation(FVector NewLoc)
{
	Multicast_SetLocation(NewLoc);
}
void APlayerCharacter::Multicast_SetLocation_Implementation(FVector NewLoc)
{
	SetActorLocation(NewLoc);
}


void APlayerCharacter::Server_RunInteraction_Implementation(AActor* SmartObj, FVector NewLoc, FRotator NewRot)
{
	if (SmartObj)
	{
		if(NewLoc != FVector::ZeroVector) Server_SetLocation(NewLoc);
		Server_SetRotation(NewRot);
		if (SmartObj->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
		{
			Cast<IInteractableInterface>(SmartObj)->PlayInteractionMt();
		}
	}
}


void APlayerCharacter::Client_PointEvent_Implementation()
{
	if (APlayerControllerBase* ConBase = Cast<APlayerControllerBase>(GetController()))
	{
		ConBase->LowShakeCamera();
	}
}
bool APlayerCharacter::Client_PointEvent_Validate()
{
	return true;
}

void APlayerCharacter::Client_CallEventUI_Implementation(EEventUIType EventType)
{
	PlayerHUD->CallEventUI(EventType);
}
bool APlayerCharacter::Client_CallEventUI_Validate(EEventUIType EventType)
{
	return true;
}

void APlayerCharacter::UpdateInputKey(EInputKey InputKey)
{
	if(!GetCanInputCheck()) return;

	InputType.AllTransFalse();
	if (static_cast<uint8>(InputKey) == static_cast<uint8>(EInputKey::None))
	{

	}
	else if (static_cast<uint8>(InputKey) == static_cast<uint8>(EInputKey::LMB))
	{
		if (HoldType == EEquipHand::None || HoldType == EEquipHand::Right)
		{
			if (AnimValues.bLowAttack || AnimValues.bHeavyAttack || AnimValues.bDualAttack)
			{
				InputType.bButtonWhenAttack = true;
			}
			else if (AnimValues.bSpellAttack)
			{
				if (HoldType == EEquipHand::Left)  return;
				if (MainWeapon.IsSet()
					&& MainWeapon.WeaponData.WeaponType == EWeaponType::Wand
					&& (E_Spell.SpellData.SpellType == ESpellType::AttackSorcery
						|| E_Spell.SpellData.SpellType == ESpellType::AssistSorcery))
					InputType.bButtonWhenSpell = true;
				else
					InputType.bButtonWhenAttack = true;
			}
		}
		else if (HoldType == EEquipHand::Left)
		{
			if (AnimValues.bLowAttack || AnimValues.bHeavyAttack)
			{
				InputType.bButtonWhenAttack = true;
			}
			else if (AnimValues.bSpellAttack)
			{
				if (HoldType == EEquipHand::Left)
					if (SecondaryWeapon.IsSet()
						&& SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Talisman
						&& (E_Spell.SpellData.SpellType == ESpellType::AttackIncantation
							|| E_Spell.SpellData.SpellType == ESpellType::AssistIncantation))
						InputType.bButtonWhenSpell = true;
					else
						InputType.bButtonWhenAttack = true;
			}
		}
	}
	else if (static_cast<uint8>(InputKey) == static_cast<uint8>(EInputKey::RMB))
	{
		if (HoldType == EEquipHand::None)
		{
			if (MainWeapon.IsSet() && GetPowerup())
			{
				InputType.bButtonWhenAOE = true;
			}
			else if (SecondaryWeapon.IsSet())
			{
				if(SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Talisman
				&& (E_Spell.SpellData.SpellType == ESpellType::AttackIncantation
					|| E_Spell.SpellData.SpellType == ESpellType::AssistIncantation))
				{
					InputType.bButtonWhenSpell = true;
					return;
				}
				else
				{
					if (GetPowerup())
					{
						InputType.bButtonWhenAOE = true;
					}
					else
					{
						if (SecondaryWeapon.WeaponData.WeaponType == EWeaponType::Strike)
							InputType.bButtonWhenDualAttack = true;
					}
				}
			}
		}
		else
		{
			if(GetPowerup()) InputType.bButtonWhenAOE = true;
		}
	}
	else if (static_cast<uint8>(InputKey) == static_cast<uint8>(EInputKey::SPB))
	{
		InputType.bButtonWhenRoll = true;
	}
	else if (static_cast<uint8>(InputKey) == static_cast<uint8>(EInputKey::RKB))
	{
		InputType.bButtonWhenExpendable = true;
	}
}

//Mt ���� �ٴٸ��� ȣ���Լ�//
void APlayerCharacter::EndMt()
{
	SetCanInputCheck(false);

	CurrentLowAttackCount = 0;
	CurrentAOECount = 0;

	AnimValues.AllTransFalse();
	InputType.AllTransFalse();

	IncreaseData.ResetValue();
	SpellData.ResetValue();
	bCharging = false;
	//bDead = false;
	bUseOrientYaw = false;


	if (GetCombatComponent() && GetStateComponent())
	{
		GetStateComponent()->ResetPoiseStat();
		GetCombatComponent()->SetFalseHitSurface();
		//GetCombatComponent()->SetExecutionType(EExecutionType::None, nullptr);
	}
}

void APlayerCharacter::Server_CallMessageUI_Implementation(FName OutputName)
{
	Client_CallMessageUI(OutputName);
}
bool APlayerCharacter::Server_CallMessageUI_Validate(FName OutputName)
{
	return true;
}

void APlayerCharacter::Client_CallMessageUI_Implementation(FName OutputName)
{
	if (GetHUD())
	{
		GetHUD()->CallMessageUI(OutputName);
		//FadeInOut(true);
	}
}
bool APlayerCharacter::Client_CallMessageUI_Validate(FName OutputName)
{
	return true;
}

void APlayerCharacter::Client_OverlapInteraction_Implementation(AActor* OverlapActor)
{
	if(!OverlapActor) return;
	if (OverlapActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		TArray<AActor*> OverlapActors;
		GetOverlappingActors(OverlapActors);
	
		if(OverlapActors.Num() == 1)
		{
			InteractionIndex = 0;
			//Server_OverlapActors(true, GetInteractionText(OverlapActors[InteractionIndex]));
			Client_OverlapActors(true, GetInteractionText(OverlapActors[InteractionIndex]));

		}
	}
}

void APlayerCharacter::Client_EndOverlapInteraction_Implementation(AActor* OverlapActor)
{
	if (!OverlapActor) return;
	if (OverlapActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		Cast<IInteractableInterface>(OverlapActor)->UpdatedOverlapEnd();
	
		TArray<AActor*> OverlapActors;
		GetOverlappingActors(OverlapActors);
	
		if (OverlapActors.Num() > 0)
		{
			for (int8 i = 0; i < OverlapActors.Num(); i++)
			{
				if(OverlapActor == OverlapActors[i]) continue;
				if (IsInteractionActor(OverlapActors[i]))
				{
					//Server_OverlapActors(true, GetInteractionText(OverlapActors[i]));
					Client_OverlapActors(true, GetInteractionText(OverlapActors[i]));
					return;
				}
			}
			//Server_OverlapActors(false);
			Client_OverlapActors(false);
		}
		else if (OverlapActors.Num() == 0)
		{
			//Server_OverlapActors(false);
			Client_OverlapActors(false);

		}
	}
}
