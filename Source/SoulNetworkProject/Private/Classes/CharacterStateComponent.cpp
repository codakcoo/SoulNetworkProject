// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/CharacterStateComponent.h"
#include "Classes/CharacterBase.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/EffectComponent.h"
#include "Widget/UI/HealthBarWidget.h"
#include "Interface/CharacterInterface.h"

#include "SoulNetworkProject/ItemState.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"


#define HEALTH TEXT("Health")
#define MANA TEXT("Mana")
#define STAMINA TEXT("Stamina")

// Sets default values for this component's properties
UCharacterStateComponent::UCharacterStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicated(true);
}


// Called when the game starts
void UCharacterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Owner = Cast<ACharacterBase>(GetOwner());
	//Server_BindProperty();
	if (Owner)
	{
		SetupState();
		if(!Owner->GetPlayerState()) return;
		if (Owner->GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			Cast<ICharacterInterface>(Owner->GetPlayerState())->SetupState(Cast<APlayerCharacter>(Owner));


		}
	}
}


// Called every frame
void UCharacterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	
	if (Owner)
	{
		
	}
		UpdateStamina(DeltaTime);

}

void UCharacterStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterStateComponent, Owner);
	DOREPLIFETIME(UCharacterStateComponent, OperatorType);

	DOREPLIFETIME(UCharacterStateComponent, MaxStat);
	DOREPLIFETIME(UCharacterStateComponent, CurrentStat);

	DOREPLIFETIME_CONDITION(UCharacterStateComponent, bUseHyperArmor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UCharacterStateComponent, bUseRigidity, COND_OwnerOnly);
	DOREPLIFETIME(UCharacterStateComponent, bDamagePossible);

	DOREPLIFETIME(UCharacterStateComponent, EnchantBuff);
	DOREPLIFETIME(UCharacterStateComponent, EnforceBuffs);

	DOREPLIFETIME(UCharacterStateComponent, DecreaseHealth);
}


void UCharacterStateComponent::AddHealth(float InHealth)
{
	if (Owner->GetLocalRole() == ROLE_Authority)
	{
		CurrentStat.Health += InHealth;
		if (CurrentStat.Health <= 0.f) CurrentStat.Health = 0.f;
		else if (CurrentStat.Health >= MaxStat.Health) CurrentStat.Health = MaxStat.Health;

		//OnHealthUpdated.Broadcast(InHealth < 0.0f ? true : false);
		Client_PercentBroadcast(HEALTH, InHealth < 0.0f ? true : false);

		DecreaseHealth += (int32)FMath::Abs(InHealth);
		SoulDebug::ScreenLog(FString::Printf(TEXT("%f"), CurrentStat.Mana), FColor::Blue, 1.0f);


		if(GetWorld()->GetTimerManager().IsTimerActive(DecreaseHandle)) GetWorld()->GetTimerManager().ClearTimer(DecreaseHandle);
		GetWorld()->GetTimerManager().SetTimer(DecreaseHandle, FTimerDelegate::CreateWeakLambda(this, [&]
		{
			DecreaseHealth = 0;
		}), DecreaseDelay, false);
	}
	else if (Owner->GetRemoteRole() == ROLE_AutonomousProxy)
	{
		Server_AddHealth(InHealth);
	}
}

void UCharacterStateComponent::Server_AddHealth_Implementation(float InHealth)
{
	//CurrentStat.Health += InHealth;
	//if (CurrentStat.Health <= 0.f) CurrentStat.Health = 0.f;
	//else if (CurrentStat.Health >= MaxStat.Health) CurrentStat.Health = MaxStat.Health;

	AddHealth(InHealth);
}
bool UCharacterStateComponent::Server_AddHealth_Validate(float InHealth)
{
	return true;
}

void UCharacterStateComponent::AddMana(float InMana)
{

	if (Owner->GetLocalRole() == ROLE_Authority)
	{
		CurrentStat.Mana += InMana;
		if(CurrentStat.Mana <= 0.f) CurrentStat.Mana = 0.f;
		else if (CurrentStat.Mana >= MaxStat.Mana) CurrentStat.Mana = MaxStat.Mana;

		//OnManaUpdated.Broadcast(InMana < 0.0f ? true : false);
		Client_PercentBroadcast(MANA, InMana < 0.0f ? true : false);

		SoulDebug::ScreenLog(FString::Printf(TEXT("%f"), CurrentStat.Mana), FColor::Green, 1.0f);
	}
	else if (Owner->GetRemoteRole() == ROLE_AutonomousProxy)
	{
		Server_AddMana(InMana);
	}
}

void UCharacterStateComponent::Server_AddMana_Implementation(float InMana)
{
	//CurrentStat.Mana += InMana;
	//if (CurrentStat.Mana <= 0.f) CurrentStat.Mana = 0.f;
	//else if (CurrentStat.Mana >= MaxStat.Mana) CurrentStat.Mana = MaxStat.Mana;
	AddMana(InMana);
}
bool UCharacterStateComponent::Server_AddMana_Validate(float InMana)
{
	return true;
}

void UCharacterStateComponent::AddStamina(float InStamina)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		SetOperator(EOperatorType::Stop);
		CurrentStat.Stamina += InStamina;
		if(CurrentStat.Stamina <= 0.f) CurrentStat.Stamina = 0.f;
		else if (CurrentStat.Stamina >= MaxStat.Stamina) CurrentStat.Stamina = MaxStat.Stamina;

		//OnStaminaUpdated.Broadcast(InStamina < 0.0f ? true : false);
		Client_PercentBroadcast(STAMINA, InStamina < 0.0f ? true : false);

	}
	else if (Owner->GetRemoteRole() == ROLE_AutonomousProxy)
	{
		Server_AddStamina(InStamina);
	}
}

void UCharacterStateComponent::Server_AddStamina_Implementation(float InStamina)
{
	//SetOperator(EOperatorType::Stop);
	//CurrentStat.Stamina += InStamina;
	//if (CurrentStat.Stamina <= 0.f) CurrentStat.Stamina = 0.f;
	//else if (CurrentStat.Stamina >= MaxStat.Stamina) CurrentStat.Stamina = MaxStat.Stamina;
	AddStamina(InStamina);
}
bool UCharacterStateComponent::Server_AddStamina_Validate(float InStamina)
{
	return true;
}

void UCharacterStateComponent::AddCurrentPoiseHealth(float InPoiseHealth)
{
	if(InPoiseHealth < 0.f) return;

	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrentStat.PoiseHealth += InPoiseHealth;
		if (CurrentStat.PoiseHealth <= 0.f) CurrentStat.PoiseHealth = 0.f;
	}
}

void UCharacterStateComponent::OnDamagePossible(float Delay)
{
	if (Owner->GetLocalRole() == ROLE_Authority)
	{
		bDamagePossible = false;

		GetWorld()->GetTimerManager().SetTimer(DamagePossibleHandle, FTimerDelegate::CreateWeakLambda(this, [&]
		{
			bDamagePossible = true;
		}), Delay, false);
	}
	else if(Owner->GetRemoteRole() == ROLE_AutonomousProxy)
	{
		Server_OnDamagePossible(Delay);
	}

}

void UCharacterStateComponent::SetDamagePossible(bool bActive)
{
	if (Owner->GetLocalRole() == ROLE_Authority)
	{
		if(GetWorld()->GetTimerManager().IsTimerActive(DamagePossibleHandle)) GetWorld()->GetTimerManager().ClearTimer(DamagePossibleHandle);
		bDamagePossible = bActive;
	}
	else if (Owner->GetRemoteRole() == ROLE_AutonomousProxy)
	{
		Server_SetDamagePossible(bActive);
	}
}

void UCharacterStateComponent::SetCurrentHealth_Implementation(float InValue)
{
	CurrentStat.Health = InValue;
	Client_PercentBroadcast(HEALTH, false);
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%f"), InValue));
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("%f"), CurrentStat.Health));

}

void UCharacterStateComponent::SetCurrentMana_Implementation(float InValue)
{
	CurrentStat.Mana = InValue;
	Client_PercentBroadcast(MANA, false);
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%f"), InValue));
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("%f"), CurrentStat.Mana));

}

void UCharacterStateComponent::Server_SetDamagePossible_Implementation(bool bActive)
{
	SetDamagePossible(bActive);
}
bool UCharacterStateComponent::Server_SetDamagePossible_Validate(bool bActive)
{
	return true;
}

void UCharacterStateComponent::Server_OnDamagePossible_Implementation(float Delay)
{
	//bDamagePossible = false;
	//
	//GetWorld()->GetTimerManager().SetTimer(DamagePossibleHandle, FTimerDelegate::CreateLambda([&]
	//	{
	//		bDamagePossible = true;
	//	}), Delay, false);
	Server_OnDamagePossible(Delay);
}
bool UCharacterStateComponent::Server_OnDamagePossible_Validate(float Delay)
{
	return true;
}

uint8 UCharacterStateComponent::GetNextExp(uint8 NextLevel)
{
	FChrStat* RowData = GetChrStatRow(NextLevel);

	return RowData ? RowData->NextExp : 0xff;
}

void UCharacterStateComponent::ResetPoiseStat()
{
	bUseHyperArmor = false;

	CurrentStat.PoiseHealth = MaxStat.PoiseHealth;
}

void UCharacterStateComponent::SetOperator(EOperatorType InOperatorType)
{
	if (Owner->GetLocalRole() == ROLE_Authority)
	{
		OperatorType = InOperatorType;
		if (GetWorld()->GetTimerManager().IsTimerActive(StaminaHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(StaminaHandle);
		}

		if (InOperatorType == EOperatorType::Stop)
		{
			GetWorld()->GetTimerManager().SetTimer(StaminaHandle, FTimerDelegate::CreateWeakLambda(this, [&]()
				{
					OperatorType = EOperatorType::Add;
				}), StaminaDelay, false);
		}

		//OnStaminaUpdated.Broadcast(InOperatorType == EOperatorType::Stop ? true : false);
		Client_PercentBroadcast(STAMINA, InOperatorType == EOperatorType::Stop ? true : false);
	}
	else
	{
		Server_SetOperator(InOperatorType);
	}
}

void UCharacterStateComponent::Server_SetOperator_Implementation(EOperatorType InOperatorType)
{
	//OperatorType = InOperatorType;
	//if (GetWorld()->GetTimerManager().IsTimerActive(StaminaHandle))
	//{
	//	GetWorld()->GetTimerManager().ClearTimer(StaminaHandle);
	//}
	//
	//if (InOperatorType == EOperatorType::Stop)
	//{
	//	GetWorld()->GetTimerManager().SetTimer(StaminaHandle, FTimerDelegate::CreateLambda([&]()
	//		{
	//			OperatorType = EOperatorType::Add;
	//		}), StaminaDelay, false);
	//}

	SetOperator(InOperatorType);
}
bool UCharacterStateComponent::Server_SetOperator_Validate(EOperatorType InOperatorType)
{
	return true;
}

void UCharacterStateComponent::RunBuffData(FName RowName)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		FBuffData* RowData = GetBuffDataRow(RowName);
		if (RowData)
		{
			if(RowData->RunType == EBuffRunType::Enchant)
			{
				if(EnchantBuff.IsSet()) Multicast_ClearEnchant();
				EnchantBuff = *RowData;
				OnRep_Enchant();
			}
			if(RowData->RunType == EBuffRunType::Enforce)
			{
				for (auto& Enforce : EnforceBuffs)
				{
					if (Enforce  == *RowData)
					{
						GetWorld()->GetTimerManager().ClearTimer(Enforce.BuffTimerHandle);
						EnforceBuffs.RemoveSingle(Enforce);

						break;
					}
				}

				EnforceBuffs.Add(*RowData);
				OnRep_Enforce();
			}
		}
	}
	else
	{
		Server_RowBuffData(RowName);
	}
}

void UCharacterStateComponent::Server_RowBuffData_Implementation(FName RowName)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		RunBuffData(RowName);
	}
}
bool UCharacterStateComponent::Server_RowBuffData_Validate(FName RowName)
{
	return true;
}

void UCharacterStateComponent::OnRep_Enchant()
{
	ACharacterBase* ComOnwer = Cast<ACharacterBase>(GetOwner());
	if(!ComOnwer) return;

	if (EnchantBuff.IsSet())
	{
		if (ComOnwer->GetEffectComponent())
		{

			ComOnwer->GetEffectComponent()->RunWeaopnMeshNiagara(EnchantBuff.DataName);
			GetWorld()->GetTimerManager().SetTimer(EnchantBuff.BuffTimerHandle, FTimerDelegate::CreateLambda([&]
			{
				ACharacterBase* TimerOwner = Cast<ACharacterBase>(GetOwner());
				if(!TimerOwner) return;

				TimerOwner->GetEffectComponent()->RemoveWeaponMeshNigara();
				EnchantBuff.Reset();
			}), EnchantBuff.TimerDelay, false);
		}
	}
}

void UCharacterStateComponent::OnRep_Enforce()
{
	ACharacterBase* Chr = Cast<ACharacterBase>(GetOwner());
	if(!Chr) return;

	if (EnforceBuffs.Num() > 0)
	{
		FBuffData& Data = EnforceBuffs[EnforceBuffs.Num() - 1];
		Chr->GetEffectComponent()->RunBodyMeshNiagara(Data.DataName);
		GetWorld()->GetTimerManager().SetTimer(EnforceBuffs[EnforceBuffs.Num() - 1].BuffTimerHandle, FTimerDelegate::CreateWeakLambda(this, [&Data, this]()
			{
				ACharacterBase* TimerOwner = Cast<ACharacterBase>(GetOwner());
				if (!TimerOwner) return;

				TimerOwner->GetEffectComponent()->RemoveBodyMeshNiagara(Data.DataName);
				EnforceBuffs.RemoveSingle(Data);
				Data.Reset();
			}), EnforceBuffs[EnforceBuffs.Num() - 1].TimerDelay, false);
	}
}

void UCharacterStateComponent::Multicast_ClearEnchant_Implementation()
{
	ACharacterBase* ComOwner = Cast<ACharacterBase>(GetOwner());
	if(!ComOwner) return;

	ComOwner->GetEffectComponent()->RemoveWeaponMeshNigara();
	GetWorld()->GetTimerManager().ClearTimer(EnchantBuff.BuffTimerHandle);
}
bool UCharacterStateComponent::Multicast_ClearEnchant_Validate()
{
	return true;
}

void UCharacterStateComponent::UpdateStamina(float InDeltaTime)
{
	if (Owner->GetLocalRole() == ROLE_Authority)
	{
		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green, FString::Printf(TEXT("Current Stamina: %f"), CurrentStat.Stamina));
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Max Stamina: %f"), MaxStat.Stamina));
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("Current Stamina: %f"), CurrentStat.Stamina));
		if(OperatorType == EOperatorType::Stop)
		{
			return;
		}

		if (OperatorType == EOperatorType::Add)
		{
			CurrentStat.Stamina += StaminaIncrease;


			if (CurrentStat.Stamina >= MaxStat.Stamina) OperatorType = EOperatorType::Stop;

			return;
		}
		else if (OperatorType == EOperatorType::Min)
		{
			CurrentStat.Stamina -= StaminaIncrease;

			if (CurrentStat.Stamina <= 0.f) OperatorType = EOperatorType::Stop;

			return;
		}

	}
	else
	{
		Server_UpdateStamina(InDeltaTime);
	}
}

void UCharacterStateComponent::Server_UpdateStamina_Implementation(float InDeltaTime)
{
	UpdateStamina(InDeltaTime);
}
bool UCharacterStateComponent::Server_UpdateStamina_Validate(float InDeltaTime)
{
	return true;
}

FChrStat* UCharacterStateComponent::GetChrStatRow(int8 RowLevel)
{
	if(!StatDt) return nullptr;

	static const FString ContextString(TEXT("Base Data Context"));
	FChrStat* RowData = StatDt->FindRow<FChrStat>( *FString::FromInt(RowLevel), ContextString, true);


	return RowData ? RowData : nullptr;
}

FBuffData* UCharacterStateComponent::GetBuffDataRow(FName RowName)
{
	if (!BuffDt) return nullptr;

	static const FString ContextString(TEXT("Base Data Context"));
	FBuffData* RowData = BuffDt->FindRow<FBuffData>(RowName, ContextString, true);


	return RowData ? RowData : nullptr;
}

void UCharacterStateComponent::Client_PercentBroadcast_Implementation(FName PermuteName, bool bAnim)
{
	if(PermuteName == HEALTH) OnHealthUpdated.Broadcast(bAnim);
	else if(PermuteName == MANA) OnManaUpdated.Broadcast(bAnim);
	else if(PermuteName == STAMINA) OnStaminaUpdated.Broadcast(bAnim);
}

void UCharacterStateComponent::Server_BindProperty_Implementation()
{
	Owner = Cast<APlayerCharacter>(GetOwner());
}

void UCharacterStateComponent::SetupState()
{
	//if(!Owner) return;

	if (Owner->GetLocalRole() == ROLE_Authority)
	{
		if (StatDt)
		{
			FChrStat* RowData = GetChrStatRow(Owner->GetLevel());
			if (RowData)
			{
				//MaxStat.Health = RowData->Health;
				//MaxStat.Mana = RowData->Mana;
				//MaxStat.Stamina = RowData->Stamina;
				//
				//
				//CurrentStat.Health = RowData->Health;
				//CurrentStat.Mana = RowData->Mana;
				//CurrentStat.Stamina = RowData->Stamina;
				MaxStat = *RowData;
				CurrentStat = *RowData;
			}
		}
	}
	else
	{
		Server_SetupState();
	}
}

void UCharacterStateComponent::UpdatedIncreaseData()
{
	if (Owner->GetLocalRole() == ROLE_Authority)
	{
		if (Owner->GetIncreaseData().IsSet() && Owner->GetIncreaseData().ItemCount > 0)
		{
			switch (Owner->GetIncreaseData().ExpendableData.ElementType)
			{
				case EExpendableElementType::Health:
				{
					AddHealth(Owner->GetIncreaseData().ExpendableData.IncreaseValue);
					Owner->GetEffectComponent()->EffectSequence(EEffectType::Health, FVector::ZeroVector);
				}
				break;
				case EExpendableElementType::Mana:
				{
					AddMana(Owner->GetIncreaseData().ExpendableData.IncreaseValue);
					Owner->GetEffectComponent()->EffectSequence(EEffectType::Mana, FVector::ZeroVector);
				}
				break;
				case EExpendableElementType::STR:
				{

				}
				break;
				case EExpendableElementType::INT:
				{

				}
				break;
				case EExpendableElementType::Fire:
				{

				}
				break;
				case EExpendableElementType::Electric:
				{

				}
				break;
			}
		}
	}
	else
	{
		Server_UpdatedIncreaseData();
	}
}

void UCharacterStateComponent::Server_UpdatedIncreaseData_Implementation()
{
	UpdatedIncreaseData();
}

void UCharacterStateComponent::Server_SetupState_Implementation()
{
	SetupState();
}
bool UCharacterStateComponent::Server_SetupState_Validate()
{
	return true;
}
