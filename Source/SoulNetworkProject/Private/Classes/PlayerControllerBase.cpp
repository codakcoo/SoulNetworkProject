// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/PlayerControllerBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerCameraManagerBase.h"
#include "Classes/LockonSystemComponent.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/CombatComponent.h"
#include "Classes/EffectComponent.h"
#include "SaveGame/SaveCharacterState.h"
//#include "SoulNetworkProject/InGameModeBase.h"

#include "SoulNetworkProjectPlayerState.h"

#include "Interface/InteractableInterface.h"
#include "Interface/SaveInterface.h"
#include "Interface/LoadInterface.h"
#include "Interface/MenuInterface.h"

#include "Gameframework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

APlayerControllerBase::APlayerControllerBase(FObjectInitializer const& Object_Initializer) : Super(Object_Initializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
	//PossessPawn = Cast<APlayerCharacter>(GetPawn());

	if (PossessPawn && IsLocalController())
	{
		PossessPawn->BindProperty(this, Cast<APlayerHUD>(GetHUD()));
		PossessPawn->GetHUD()->SetupMenus(PossessPawn);

		PCM = Cast<APlayerCameraManagerBase>(PlayerCameraManager);
		PCM->BindPossessPawn(PossessPawn);
		PCM->InGameView();

		Server_RefreshMeshs();
		//StartFunction(true, true, false);
	}
}

void APlayerControllerBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APlayerControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetPossessPawn())
	{
		//Server_UpdatedHoldKey(DeltaTime);
		UpdateHoldKey(DeltaTime);

		if(GetPossessPawn()->GetDead()) return;
		if (!GetWorld()->GetTimerManager().IsTimerActive(LastLocationHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(LastLocationHandle, FTimerDelegate::CreateWeakLambda(this, [&]
			{
				if(GetPossessPawn()->GetDead())
				{
					GetWorld()->GetTimerManager().ClearTimer(LastLocationHandle);
					return;
				}
				
				if (GetPossessPawn()->UpdatedLastLocation() != FVector::ZeroVector)
				{
					LastLocation = GetPossessPawn()->UpdatedLastLocation();
				}
				//GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::White,__FUNCTION__);
			}), UpdateLocationDelay, true);
		}
	}
}

void APlayerControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerControllerBase, PlayerName);
	DOREPLIFETIME(APlayerControllerBase, PossessPawn);
	DOREPLIFETIME(APlayerControllerBase, bPlayingExecution);
}

void APlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessPawn = Cast<APlayerCharacter>(InPawn);
	//Server_PossessPawn(InPawn);
}

void APlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		//Axis//
		InputComponent->BindAxis("MoveForward", this, &APlayerControllerBase::MoveForward);
		InputComponent->BindAxis("MoveRight", this, &APlayerControllerBase::MoveRight);

		InputComponent->BindAxis("LookUp", this, &APlayerControllerBase::Lookup);
		InputComponent->BindAxis("Turn", this, &APlayerControllerBase::Turn);
		////////

		//Action//
		InputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerControllerBase::SprintPressed);
		InputComponent->BindAction("Sprint", IE_Released, this, &APlayerControllerBase::SprintReleased);

		InputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerControllerBase::CrouchPressed);
		InputComponent->BindAction("Crouch", IE_Released, this, &APlayerControllerBase::CrouchReleased);

		InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerControllerBase::JumpStart);
		InputComponent->BindAction("Jump", IE_Released, this, &APlayerControllerBase::JumpEnd);

		InputComponent->BindAction("Interaction", IE_Pressed, this, &APlayerControllerBase::EKBPressed);
		InputComponent->BindAction("Interaction", IE_Released, this, &APlayerControllerBase::EKBReleased);

		InputComponent->BindAction("UseExpandable", IE_Pressed, this, &APlayerControllerBase::RKBPressed);

		InputComponent->BindAction("Lockon", IE_Pressed, this, &APlayerControllerBase::QKBPressed);

		InputComponent->BindAction("Powerup", IE_Pressed, this, &APlayerControllerBase::SfBPressed);
		InputComponent->BindAction("Powerup", IE_Released, this, &APlayerControllerBase::SfBReleased);

		InputComponent->BindAction("LMB", IE_Pressed, this, &APlayerControllerBase::LMBClicked);
		InputComponent->BindAction("LMB", IE_Released, this, &APlayerControllerBase::LMBReleased);

		InputComponent->BindAction("RMB", IE_Pressed, this, &APlayerControllerBase::RMBClicked);
		InputComponent->BindAction("RMB", IE_Released, this, &APlayerControllerBase::RMBReleased);

		InputComponent->BindAction("L_Switch", IE_Pressed, this, &APlayerControllerBase::SwitchLeft);
		InputComponent->BindAction("R_Switch", IE_Pressed, this, &APlayerControllerBase::SwitchRight);
		InputComponent->BindAction("U_Switch", IE_Pressed, this, &APlayerControllerBase::SwitchUp);
		InputComponent->BindAction("D_Switch", IE_Pressed, this, &APlayerControllerBase::SwitchDown);

		InputComponent->BindAction("Z_Switch", IE_Pressed, this, &APlayerControllerBase::LeftMenu);
		InputComponent->BindAction("X_Switch", IE_Pressed, this, &APlayerControllerBase::RightMenu);

		InputComponent->BindAction("ESC", IE_Pressed, this, &APlayerControllerBase::ESCPressed);
		//////////

	}
}

void APlayerControllerBase::LowShakeCamera()
{
	if (PCM) PCM->StartCameraShake(PCM->LowShake_C);
}

void APlayerControllerBase::Server_RefreshMeshs_Implementation()
{
	if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(ULoadInterface::StaticClass()))
	{
		Cast<ILoadInterface>(GetWorld()->GetAuthGameMode())->AllPlayerRefreshCloth(this);
	}
}

void APlayerControllerBase::TopView()
{
	if (PCM) PCM->LockTopView();
}

void APlayerControllerBase::OneRateCS_Implementation(EShakeType ShakeType)
{
	if(PCM)
	{
		switch (ShakeType)
		{
			case EShakeType::LowShake:
			PCM->RunLowCameraShake();
			break;

			case EShakeType::HeavyShake:
			PCM->RunHeavyCameraShake();
			break;
		}

		SoulDebug::ScreenLog(__FUNCTION__, FColor::White, 10.f);
	}
}


void APlayerControllerBase::ChangeActiveMenu()
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetHUD()))
	{
		PH->SetActiveMenu(EActiveMenu::Popup);
	}
}

void APlayerControllerBase::AllClearTimer()
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetHUD()))
	{
		PH->ClearWidgetTimer();
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		GetWorld()->GetTimerManager().ClearAllTimersForObject(GetPawn());
	}

}

void APlayerControllerBase::CallCheckPointMenu()
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetHUD()))
	{
		PH->SwitchCheckPointMenu();
	}
}

// ServerNotifyLoadedWorld : 새로운 맵으로 로드 완료했을떄 서버에게 알리는 호출함수라고 함. void ServerNotifyLoadedWorld(FName WorldPackageName)
//void APlayerControllerBase::LoadWorld_Implementation()
//{
//	if(!GetWorld()->GetAuthGameMode()) return;
//	if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(ULoadInterface::StaticClass()))
//	{
//		Cast<ILoadInterface>(GetWorld()->GetAuthGameMode())->LoadHostWorld();
//	}
//}

void APlayerControllerBase::Client_SavePlayerData_Implementation()
{
	//Cast<ISaveInterface>(GetGameInstance())->AddSaveData(GetPossessPawn()->GetInventoryComponent()->PermuteItemDataToContents(), 0, TEXT(""), FTransform());
	Cast<ISaveInterface>(GetGameInstance())->SaveInventoryData(GetPossessPawn()->GetInventoryComponent());
	Cast<ISaveInterface>(GetGameInstance())->AddSavePlayer(GetPossessPawn());
}
void APlayerControllerBase::Client_SaveLostSoul_Implementation()
{
	if (GetPossessPawn() && GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		Cast<ISaveInterface>(GetGameInstance())->AddSaveRemainSoulData(GetPossessPawn()->GetSoul(), LastLocation);
	}
}
void APlayerControllerBase::Client_ClearLostSoul_Implementation()
{
	if (GetPossessPawn() && GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		Cast<ISaveInterface>(GetGameInstance())->AddSaveRemainSoulData(GetPossessPawn()->GetSoul(), FVector::ZeroVector);
	}
}

void APlayerControllerBase::Client_SaveCurrentCondition_Implementation()
{
	if (GetPossessPawn() && 
		GetPossessPawn()->GetStateComponent() && 
		GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		Cast<ISaveInterface>(GetGameInstance())->SaveCondition(
			GetPossessPawn()->GetStateComponent()->GetCurrentHealth(),
			GetPossessPawn()->GetStateComponent()->GetCurrentMana());
	}
}
void APlayerControllerBase::Client_SaveMaxCondition_Implementation()
{
	if (GetPossessPawn() && 
		GetPossessPawn()->GetStateComponent() && 
		GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		Cast<ISaveInterface>(GetGameInstance())->SaveCondition(
			GetPossessPawn()->GetStateComponent()->GetMaxHealth(), 
			GetPossessPawn()->GetStateComponent()->GetMaxHealth());
	}
}

void APlayerControllerBase::Client_SetCurrentCondition_Implementation()
{
	if (GetPossessPawn() && 
		GetPossessPawn()->GetStateComponent() && 
		GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		USaveCharacterState* CurrentSaveData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData();
		if (CurrentSaveData)
		{
			GetPossessPawn()->GetStateComponent()->SetCurrentHealth(CurrentSaveData->CurrentHealth);
			GetPossessPawn()->GetStateComponent()->SetCurrentMana(CurrentSaveData->CurrentMana);

		}
	}
}

void APlayerControllerBase::Client_CallEventUI_Implementation(EEventUIType EventType)
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetHUD()))
	{
		PH->CallEventUI(EventType);
	}
}

void APlayerControllerBase::Client_FadeScreen_Implementation(bool bFadeIn)
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		Cast<IMenuInterface>(GetGameInstance())->ScreenFade(bFadeIn);
	}
}

void APlayerControllerBase::Client_SetBossWidget_Implementation(bool bVisible)
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetHUD()))
	{
		PH->BindBossWidget(bVisible);
	}
}

void APlayerControllerBase::SetCinematic_Implementation(bool bCinematic)
{
	if (APlayerHUD* PH = Cast<APlayerHUD>(GetHUD())) bCinematic ? PH->EnableCinematicMode() : PH->DiableCinematicMode();
}
void APlayerControllerBase::SetInput_Implementation(bool bInput)
{
	bInput ? EnableInput(this) : DisableInput(this);
}

void APlayerControllerBase::MoveForward(float AxisValue)
{
	if (CanUseInput())
	{
		PossessPawn->MoveForward(AxisValue);
		PossessPawn->SetInputValue(AxisValue, true);
	}
}

void APlayerControllerBase::MoveRight(float AxisValue)
{
	if (CanUseInput())
	{
		PossessPawn->MoveRight(AxisValue);
		PossessPawn->SetInputValue(AxisValue, false);
	}
}

void APlayerControllerBase::Lookup(float AxisValue)
{
	if (CanUseInput())
	{
		if (PossessPawn->GetLockonComponent())
		{
			PossessPawn->GetLockonComponent()->GetLockChr() ?
				//Server_AxisLock(AxisValue, true)
				GetPossessPawn()->GetLockonComponent()->CheckDirectionScreenPoint(AxisValue, true)
				: PossessPawn->LookUp(AxisValue);
		
		}
		//Server_AxisLock(AxisValue, true);
	}
}

void APlayerControllerBase::Turn(float AxisValue)
{
	if (CanUseInput())
	{
		if (PossessPawn->GetLockonComponent())
		{
			PossessPawn->GetLockonComponent()->GetLockChr() ?
				//Server_AxisLock(AxisValue, false)
				GetPossessPawn()->GetLockonComponent()->CheckDirectionScreenPoint(AxisValue, false)
				: PossessPawn->Turn(AxisValue);
		}
		//Server_AxisLock(AxisValue, false);
	}
}

void APlayerControllerBase::LMBClicked()
{
	if (!CanUseInput() || GetPossessPawn()->AnimValues.bMtMotion) return;


	if (GetPossessPawn()->GetCharacterMovement()->IsFalling())
	{
		if (!GetPossessPawn()->AnimValues.ValueCheckBoolean())
		{
			GetPossessPawn()->Server_AirAttack();
		}
	}
	else
	{
		if (GetPossessPawn()->AnimValues.ValueCheckBoolean())
		{
			GetPossessPawn()->UpdateInputKey(EInputKey::LMB);
		}
		else
		{
			if (GetPossessPawn()->GetInteract() && GetPossessPawn()->GetHoldTypeWeapon().IsSet())
			{
				if (!GetPossessPawn()->IsPlayingMontage()) GetPossessPawn()->Server_UpdatedHandType(EEquipHand::Right);

				return;
			}

			if (GetPossessPawn()->GetPowerup())
			{
				GetPossessPawn()->Server_HeavyAttackMontage();
			}
			else
			{
				GetPossessPawn()->LowAttack();
			}
			GetPossessPawn()->EndCrouch();
		}
	}
}

void APlayerControllerBase::LMBReleased()
{
	if (!CanUseInput() || GetPossessPawn()->AnimValues.bMtMotion) return;

	if (GetPossessPawn()->GetCharging())
	{
		if (GetPossessPawn()->GetMainWeapon().WeaponData.WeaponType == EWeaponType::Wand)
		{
			if (GetPossessPawn()->AnimValues.bHeavyAttack)
			{
				GetPossessPawn()->HeavyAttack();
			}
			else if (GetPossessPawn()->AnimValues.bSpellAttack && GetPossessPawn()->IsMainSpell())
			{
				GetPossessPawn()->Server_E_SpellMontage(GetPossessPawn()->GetSpellData());
			}
		}
		else if (GetPossessPawn()->GetMainWeapon().WeaponData.WeaponType == EWeaponType::Strike)
		{
			GetPossessPawn()->HeavyAttack();
		}
	}
	else
	{
		if (GetPossessPawn()->AnimValues.bAim)
		{
			if (GetPossessPawn()->IsPlayingMontage())
			{
				GetPossessPawn()->InputType.AllTransFalse();
				GetPossessPawn()->InputType.bButtonWhenAttack = true;
			}
			else
			{
				GetPossessPawn()->Server_BowMontage(PossessPawn->GetPowerup());
			}

			GetPossessPawn()->ToggleAim(false);
		}
	}
}

void APlayerControllerBase::RMBClicked()
{
	if (!CanUseInput() || GetPossessPawn()->AnimValues.bMtMotion) return;

	if (GetPossessPawn() && GetPossessPawn()->GetHUD() && !GetPossessPawn()->GetDead() && GetPossessPawn()->GetHUD()->GetActivateMenu())
	{
		return;
	}

	if (GetPossessPawn()->GetCharacterMovement()->IsFalling())
	{
		//if (!GetPossessPawn()->AnimValues.ValueCheckBoolean())
		//{
		//	//Player->Server_AirAttack();
		//}

		return;
	}
	else
	{
		if (GetPossessPawn()->AnimValues.ValueCheckBoolean())
		{
			GetPossessPawn()->UpdateInputKey(EInputKey::RMB);
		}
		else
		{
			if (GetPossessPawn()->GetInteract() && GetPossessPawn()->GetSecondaryWeapon().IsSet())
			{
				if (!GetPossessPawn()->IsPlayingMontage()) GetPossessPawn()->Server_UpdatedHandType(EEquipHand::Left);

				return;
			}
			else
			{
				if (GetPossessPawn()->GetPowerup())
				{
					GetPossessPawn()->AOEAttack();
				}
				else
				{
					EWeaponType WeaponType = GetPossessPawn()->GetSecondaryWeapon().WeaponData.WeaponType;

					if (WeaponType == EWeaponType::Bow)
					{
						//GetPossessPawn()->GetHoldType() == EEquipHand::None ?
							//[&] {GetPossessPawn()->Server_UpdatedHandType(EEquipHand::Left); }() :
							//[&] {GetPossessPawn()->ToggleAim(true); GetPossessPawn()->LowAttack(); }();
						if(GetPossessPawn()->GetHoldType() == EEquipHand::None)
							GetPossessPawn()->Server_UpdatedHandType(EEquipHand::Left);
					}
					else if (WeaponType == EWeaponType::Shield)
					{
						GetPossessPawn()->ToggleGuard(true);
					}
					else if (WeaponType == EWeaponType::Talisman && GetPossessPawn()->IsSecondarySpell())
					{
						GetPossessPawn()->TalismanAttack();
						//if(GetPossessPawn()->GetHoldType() == EEquipHand::None) GetPossessPawn()->TalismanAttack();
						//else
						//{
						//	if(GetPossessPawn()->GetHoldTypeWeapon().WeaponData.WeaponType == EWeaponType::Talisman) GetPossessPawn()->TalismanAttack();
						//}
					}
					else if (WeaponType== EWeaponType::Strike)
					{
						if(GetPossessPawn()->GetHoldType() == EEquipHand::None) GetPossessPawn()->Server_DualAttackMontage();
						else GetPossessPawn()->ToggleGuard(true);
					}

					PossessPawn->EndCrouch();
				}
			}
		}
	}

	if (!GetPossessPawn()->CanCrouch()) GetPossessPawn()->EndCrouch();
}

void APlayerControllerBase::RMBReleased()
{
	if (!CanUseInput() || GetPossessPawn()->AnimValues.bMtMotion) return;

	if (GetPossessPawn())
	{
		if (GetPossessPawn()->AnimValues.bGuardUp)
		{
			GetPossessPawn()->ToggleGuard(false);
		}
		else
		{
			if (GetPossessPawn()->GetSecondaryWeapon().IsSet())
			{
				if (GetPossessPawn()->GetSecondaryWeapon().WeaponData.WeaponType == EWeaponType::Talisman)
				{
					if (GetPossessPawn()->GetCharging() && GetPossessPawn()->IsSecondarySpell())
					{
						GetPossessPawn()->Server_E_SpellMontage(GetPossessPawn()->GetSpellData());
					}
				}
			}
		}
		
	}
}

void APlayerControllerBase::JumpStart()
{
	if (CanUseInput() && !GetPossessPawn()->GetDead() && GetPossessPawn()->GetHUD())
	{
		if (GetPossessPawn()->GetHUD()->GetActiveMenu() != EActiveMenu::PlayerMenu)
		{
			GetPossessPawn()->GetHUD()->TotalApplyMenu();

		}
		else
		{
			GetPossessPawn()->JumpStart();
			PossessPawn->GetStateComponent()->AddStamina(-10.0f);
		}
	}
}

void APlayerControllerBase::JumpEnd()
{
	if (CanUseInput())
	{
		GetPossessPawn()->JumpEnd();
	}
}

void APlayerControllerBase::SprintPressed()
{
	if (Cast<APlayerHUD>(GetHUD())->GetActiveMenu() == EActiveMenu::Cinematic)
	{
		Cast<APlayerHUD>(GetHUD())->SetPressedOnCinematicMode(true);
		return;
	}

	if (GetPossessPawn()->AnimValues.ValueCheckBoolean())
	{
		//if (GetPossessPawn()->GetCanInputCheck())
		//{
		//	GetPossessPawn()->InputType.AllTransFalse();
		//	GetPossessPawn()->InputType.bButtonWhenRoll = true;
		//}

		GetPossessPawn()->UpdateInputKey(EInputKey::SPB);

	}
	else
	{
		if (GetPossessPawn() && !GetPossessPawn()->GetDead())
		{
			ToggleKeyState(EHoldKeyState::Space);
		}
	}
}

void APlayerControllerBase::SprintReleased()
{
	if (GetPossessPawn() && !GetPossessPawn()->GetDead())
	{
		//Player->ReleaseSpace();

		if (Cast<APlayerHUD>(GetHUD())->GetActiveMenu() == EActiveMenu::Cinematic)
		{
			Cast<APlayerHUD>(GetHUD())->SetPressedOnCinematicMode(false);
			return;
		}

		if (KeyState != EHoldKeyState::Space) return;

		if (GetPossessPawn()->GetCharacterMovement()->IsFalling() == false)
		{
			if (!GetPossessPawn()->AnimValues.ValueCheckBoolean())
			{
				if (CurrentSprintTime < MaxHoldTime)
				{
					if (GetPossessPawn()->GetStateComponent()->GetCurrentStamina() <= 0) return;

					GetPossessPawn()->InputValueRollNDodge();
				}
				else
				{
					GetPossessPawn()->GetStateComponent()->SetOperator(EOperatorType::Add);
				}
				if (!GetPossessPawn()->CanCrouch()) GetPossessPawn()->EndCrouch();
			}
			else
			{
				if (CurrentSprintTime < MaxHoldTime)
				{

					GetPossessPawn()->UpdateInputKey(EInputKey::SPB);
				}
			}
		}
		if (KeyState == EHoldKeyState::Space) ToggleKeyState(EHoldKeyState::None);
	}
}

void APlayerControllerBase::CrouchPressed()
{
	if (CanUseInput())
	{
		ToggleKeyState(EHoldKeyState::Ctrl);
	}
}

void APlayerControllerBase::CrouchReleased()
{
	if (CanUseInput())
	{
		if (KeyState != EHoldKeyState::Ctrl) return;
		if (CurrentSlowTime >= MaxHoldTime)
		{
			GetPossessPawn()->GetCharacterMovement()->MaxWalkSpeed = GetPossessPawn()->GetWalkSpeed();

		}
		else if (CurrentSlowTime < MaxHoldTime)
		{
			GetPossessPawn()->CanCrouch() ? GetPossessPawn()->StartCrouch() : GetPossessPawn()->EndCrouch();
			if (GetPossessPawn()->AnimValues.bGuardUp) GetPossessPawn()->ToggleGuard(false);
		}
		if (KeyState == EHoldKeyState::Ctrl) ToggleKeyState(EHoldKeyState::None);
	}
}

void APlayerControllerBase::QKBPressed()
{
	if (CanUseInput())
	{
		if (GetPossessPawn()->GetHUD()->GetActiveMenu() != EActiveMenu::PlayerMenu)
		{
			GetPossessPawn()->GetHUD()->TotalReverseMenu();

		}
		else
		{
			//Server_Lockon();
			if (GetPossessPawn()->GetLockonComponent() && !GetPossessPawn()->GetStartingStand())
			{
				GetPossessPawn()->GetLockonComponent()->StartLock();
			}
		}
	}
}

void APlayerControllerBase::EKBPressed()
{
	if (CanUseInput())
	{
		if (GetPossessPawn()->GetHUD()->GetActiveMenu() == EActiveMenu::Popup) return;
		else if (GetPossessPawn()->GetHUD()->GetActiveMenu() == EActiveMenu::InGameMenu)
		{
			GetPossessPawn()->GetHUD()->TotalApplyMenu();
		}
		else
		{
			//Player->Interaction(true);

			if (GetPossessPawn()->AnimValues.ValueCheckBoolean()) return;

			TArray<AActor*> OverlapActors;
			GetPossessPawn()->GetOverlappingActors(OverlapActors);

			if (OverlapActors.Num() == 0)
			{
				GetPossessPawn()->SetInteract(true);
			}
			else
			{
				if (!GetPossessPawn()->IsInteractionActor(OverlapActors[GetPossessPawn()->GetInteractionIndex()])) return;

				if (!GetPossessPawn()->GetCharacterMoving() && GetPossessPawn()->GetInventoryComponent())
				{
					if (OverlapActors[GetPossessPawn()->GetInteractionIndex()]->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
					{
						Cast<IInteractableInterface>(OverlapActors[GetPossessPawn()->GetInteractionIndex()])->Interact(GetPossessPawn());

						GetPossessPawn()->SetInteractionIndex(GetPossessPawn()->GetInteractionIndex() - 1);
						if (GetPossessPawn()->GetInteractionIndex() < 0) GetPossessPawn()->SetInteractionIndex(0);
					}
				}
			}
		}
	}
}

void APlayerControllerBase::EKBReleased()
{
	if (CanUseInput())
	{
		GetPossessPawn()->SetInteract(false);
	}
}

void APlayerControllerBase::RKBPressed()
{
	//if (GetPossessPawn() && !GetPossessPawn()->GetDead())
	//{
	//	if (GetPossessPawn()->GetExpendableData().IsSet() && !GetPossessPawn()->GetCharacterMovement()->IsFalling())
	//	{
	//		GetPossessPawn()->Server_UseExpendable(GetPossessPawn()->GetExpendableData());
	//	}
	//}
	Server_UseExpendable();
}

void APlayerControllerBase::SfBPressed()
{
	if (CanUseInput())
	{
		if (GetPossessPawn())
		{
			GetPossessPawn()->SetPowerup(true);
		}
	}
}

void APlayerControllerBase::SfBReleased()
{
	if (CanUseInput())
	{
		if (GetPossessPawn())
		{
			GetPossessPawn()->SetPowerup(false);
		}
	}

}

void APlayerControllerBase::SwitchLeft()
{
	if (CanUseInput())
	{
		if (GetPossessPawn())
		{
			SwitchAction(-1.f);
		}
	}
}

void APlayerControllerBase::SwitchRight()
{
	if (CanUseInput())
	{
		if (GetPossessPawn())
		{
			SwitchAction(1.f);
		}
	}
}

void APlayerControllerBase::SwitchUp()
{
	if (CanUseInput() && GetPossessPawn()->GetInventoryComponent())
	{
		
		GetPossessPawn()->GetInventoryComponent()->SwitchSpell();
	}
}

void APlayerControllerBase::SwitchDown()
{
	if (CanUseInput() && GetPossessPawn()->GetInventoryComponent())
	{
		GetPossessPawn()->GetInventoryComponent()->SwitchExpendable();
	}
}

void APlayerControllerBase::SwitchAction(float AxisValue)
{
	if (GetPossessPawn()->AnimValues.bMtMotion || GetPossessPawn()->IsPlayingMontage() || !CanUseInput()) return;

	if (GetPossessPawn()->GetActiveInteraction())
	{
		TArray<AActor*> OverlapActors;
		GetPossessPawn()->GetOverlappingActors(OverlapActors);
		if (OverlapActors.Num() == 1) return;


		GetPossessPawn()->SetInteractionIndex(GetPossessPawn()->GetInteractionIndex() + AxisValue > 0.f ? 1 : -1);
		if (GetPossessPawn()->GetInteractionIndex() == OverlapActors.Num()) GetPossessPawn()->SetInteractionIndex(0);
		if (GetPossessPawn()->GetInteractionIndex() < 0) GetPossessPawn()->SetInteractionIndex(OverlapActors.Num() - 1);

		GetPossessPawn()->Server_OverlapActors(true, GetPossessPawn()->GetInteractionText(OverlapActors[GetPossessPawn()->GetInteractionIndex()]));
	}
	else
	{
		AxisValue >  0.f ? 
			GetPossessPawn()->GetInventoryComponent()->SwitchRightWeapon() : 
			GetPossessPawn()->GetInventoryComponent()->SwitchLeftWeapon();
		GetPossessPawn()->Server_WeaponChangeMontage(AxisValue);
		GetPossessPawn()->Server_UpdatedHandType(EEquipHand::None);

		//Multicast_RefreshWeapon();
	}
}

void APlayerControllerBase::ESCPressed()
{
	if (CanUseInput())
	{
		GetPossessPawn()->GetHUD()->OnOffInGame();
	}
}

void APlayerControllerBase::LeftMenu()
{
	if (CanUseInput())
	{
		GetPossessPawn()->GetHUD()->TotalSwitchMenu(false);
	}
}

void APlayerControllerBase::RightMenu()
{
	if (CanUseInput())
	{
		GetPossessPawn()->GetHUD()->TotalSwitchMenu(true);
	}
}

void APlayerControllerBase::UpdateHoldKey(float InDeltaTime)
{
	if(!GetPossessPawn()) return;

	
	if(GetPossessPawn()->GetCharacterMovement()->IsFalling())// || GetPossessPawn()->IsPlayingMontage())
	{
		GetPossessPawn()->GetStateComponent()->SetOperator(EOperatorType::Stop);

		return;
	}

	if (GetPossessPawn()->AnimValues.GetSlowValue())
	{
		GetPossessPawn()->Server_ToggleSpeed(GetPossessPawn()->GetSlowSpeed());

		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("MaxSpeed: %f"), GetPossessPawn()->GetMovementComponent()->GetMaxSpeed()));

		return;
	}
	if (KeyState == EHoldKeyState::None)
	{
		//if (!GetPossessPawn()->CanCrouch()) return;

		GetPossessPawn()->Server_ToggleSpeed(GetPossessPawn()->GetWalkSpeed());
	}


	if (KeyState != EHoldKeyState::Ctrl) CurrentSlowTime = 0.f;
	else
	{
		CurrentSlowTime += InDeltaTime;
		if (CurrentSlowTime >= MaxHoldTime)
		{
			GetPossessPawn()->Server_ToggleSpeed(GetPossessPawn()->GetSlowSpeed());

			if (GetPossessPawn()->GetInputX() != 0.f || GetPossessPawn()->GetInputY() != 0.f)
			{
				GetPossessPawn()->GetStateComponent()->SetOperator(EOperatorType::Add);
			}

		}
	}

	if (KeyState != EHoldKeyState::Space) CurrentSprintTime = 0.f;
	else
	{
		CurrentSprintTime += InDeltaTime;
		if (CurrentSprintTime >= MaxHoldTime)
		{
			if (GetPossessPawn()->GetStateComponent()->GetCurrentStamina() <= 0)
			{
				GetPossessPawn()->Server_ToggleSpeed(GetPossessPawn()->GetWalkSpeed());
				KeyState = EHoldKeyState::None;
				GetPossessPawn()->GetStateComponent()->SetOperator(EOperatorType::Add);

			}
			else
			{
				GetPossessPawn()->Server_ToggleSpeed(GetPossessPawn()->GetSprintSpeed());
				if (GetPossessPawn()->GetInputX() != 0.f || GetPossessPawn()->GetInputY() != 0.f)
				{
					GetPossessPawn()->GetStateComponent()->SetOperator(EOperatorType::Min);
				}
				else
				{
					GetPossessPawn()->GetStateComponent()->SetOperator(EOperatorType::Stop);
					KeyState = EHoldKeyState::None;
				}
				
			}
		}
	}
}

void APlayerControllerBase::ToggleKeyState(EHoldKeyState ChangeState)
{
	if (GetPossessPawn()->GetStartingStand()) return;

	KeyState = ChangeState;
}


void APlayerControllerBase::Server_KilledGM_Implementation(APlayerControllerBase* Con)
{
	if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(ULoadInterface::StaticClass()))
	{
		Cast<ILoadInterface>(GetWorld()->GetAuthGameMode())->KillMessage(Con);
	}
}

void APlayerControllerBase::Server_SendJoinMessage_Implementation()
{
	if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(ULoadInterface::StaticClass()))
	{
		Cast<ILoadInterface>(GetWorld()->GetAuthGameMode())->JoinMessage(this);
	}
}

void APlayerControllerBase::Server_Lockon_Implementation()
{
	if (GetPossessPawn()->GetLockonComponent() && !GetPossessPawn()->GetStartingStand())
	{
		GetPossessPawn()->GetLockonComponent()->StartLock();
	}


	//if (GetPossessPawn()->GetAimComponent() && !GetPossessPawn()->GetStartingStand())
	//{
	//	GetPossessPawn()->GetAimComponent()->Server_StartAim();
	//}
}

void APlayerControllerBase::Server_AxisLock_Implementation(float AxisValue, bool bVertical)
{
	if (GetPossessPawn()->GetLockonComponent())
	{
		//GetPossessPawn()->GetLockonComponent()->CheckDirectionEnemy(AxisValue, bVertical);

		GetPossessPawn()->GetLockonComponent()->CheckDirectionScreenPoint(AxisValue, bVertical);
	}
}

void APlayerControllerBase::Server_UseExpendable_Implementation()
{
	if (GetPossessPawn() && !GetPossessPawn()->GetDead())
	{
		if (GetPossessPawn()->GetExpendableData().IsSet() && !GetPossessPawn()->GetCharacterMovement()->IsFalling())
		{
			GetPossessPawn()->Server_UseExpendable(GetPossessPawn()->GetExpendableData());
		}
	}
}

void APlayerControllerBase::BroadcastMessage_Implementation(FName DyingMessage)
{
	if (GetHUD() && GetPawn())
	{
		Cast<APlayerHUD>(GetHUD())->CallMessageUI(DyingMessage);
		//Cast<APlayerCharacter>(GetPawn())->Client_CallMessageUI(DyingMessage);
	}
}

void APlayerControllerBase::BroadcastCloth_Implementation()
{
	if (PossessPawn)
	{
		PossessPawn->GetClothComponent()->RefreshClothMesh();
	}
}

void APlayerControllerBase::PlayFromEndDeathEvent_Implementation()
{
	if (GetPossessPawn())
	{
		if (GetPossessPawn()->GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			Cast<ICharacterInterface>(GetPossessPawn()->GetPlayerState())->DeathEvent();
		}
	}
}

void APlayerControllerBase::Server_ToggleKeyState_Implementation(EHoldKeyState ChangeState)
{
	Multicast_ToggleKeyState(ChangeState);
}
void APlayerControllerBase::Multicast_ToggleKeyState_Implementation(EHoldKeyState ChangeState)
{
	KeyState = ChangeState;
}

bool APlayerControllerBase::CanUseInput()
{
	return PossessPawn && !PossessPawn->GetDead() && Cast<APlayerHUD>(GetHUD())->GetActiveMenu() != EActiveMenu::Cinematic;
}
