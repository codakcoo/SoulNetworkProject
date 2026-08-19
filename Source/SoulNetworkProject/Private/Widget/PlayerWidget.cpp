// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PlayerWidget.h"
#include "Widget/Slot/SlotWidget.h"
#include "Widget/UI/ItemViewWidget.h"
#include "Widget/InGameMenu.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/BossCharacter.h"
#include "Classes/CharacterStateComponent.h"
#include "Classes/Interaction/SignPointActor.h"
#include "Widget/Slot/UseItemWidget.h"


#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


UPlayerWidget::UPlayerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ViewRow_Class(TEXT("WidgetBlueprint'/Game/Widget/UI/ItemViewWidget_BP.ItemViewWidget_BP_C'"));
	if (ViewRow_Class.Succeeded())
	{
		ViewRowClass = ViewRow_Class.Class;
	}
}

bool UPlayerWidget::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	VisibilityDelegate.BindUFunction(this, "IsActiveInGameMenu");

	if(HealthBar) HealthBar->PercentDelegate.BindUFunction(this, "GetHealthPercent");
	if(ManaBar) ManaBar->PercentDelegate.BindUFunction(this, "GetManaPercent");
	if(StaminaBar) StaminaBar->PercentDelegate.BindUFunction(this, "GetStaminaPercent");

	if(L_WeaponSlot) L_WeaponSlot->bIsEnabledDelegate.BindUFunction(this, "CheckLSlotEnabled");
	if(R_WeaponSlot) R_WeaponSlot->bIsEnabledDelegate.BindUFunction(this, "CheckRSlotEnabled");
	if(SpellSlot) SpellSlot->bIsEnabledDelegate.BindUFunction(this, "CheckSpellSlotEnabled");

	if(ActionBox) ActionBox->VisibilityDelegate.BindUFunction(this, "IsTwiceToOverlap");

	if(Boss_HpBar) Boss_HpBar->PercentDelegate.BindUFunction(this, "CheckBossHpValue");
	if(Boss_DecreaseCount) Boss_DecreaseCount->TextDelegate.BindUFunction(this, "UpdatedDecreaseHealth");
	if(InteractionBox) InteractionBox->bIsEnabledDelegate.BindUFunction(this, "InteractionEnabled");

	if(ExpendableText) ExpendableText->TextDelegate.BindUFunction(this, "UpdatedExpendableText");
	if(MagicText) MagicText->TextDelegate.BindUFunction(this, "UpdatedMagicText");



	return true;
}

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		Owner = Player;
		if (Owner)
		{
			Owner->GetInventoryComponent()->OnEquipSlotUpdated.AddDynamic(this, &UPlayerWidget::UpdatedSlot);
			Owner->GetInventoryComponent()->OnViewListUpdated.AddDynamic(this, &UPlayerWidget::RefreshItemViewList);
			Owner->GetStateComponent()->OnHealthUpdated.AddDynamic(this, &UPlayerWidget::UpdateHealthText);
			Owner->GetStateComponent()->OnManaUpdated.AddDynamic(this, &UPlayerWidget::UpdateManaText);
			Owner->GetStateComponent()->OnStaminaUpdated.AddDynamic(this, &UPlayerWidget::UpdateStaminaText);
			Owner->UpdatedSoul.AddDynamic(this, &UPlayerWidget::UpdatedSoulText);

			CurrentSoul = Owner->GetSoul();
			OwnSoul = Owner->GetSoul();

			SoulText->SetText(FText::FromString(FString::FromInt(CurrentSoul)));
			AddSoulText->SetVisibility(ESlateVisibility::Hidden);

			CurrentHealth = Owner->GetStateComponent()->GetCurrentHealth() / Owner->GetStateComponent()->GetMaxHealth();
			CurrentMana = Owner->GetStateComponent()->GetCurrentMana() / Owner->GetStateComponent()->GetMaxMana();
			CurrentStamina = Owner->GetStateComponent()->GetCurrentStamina() / Owner->GetStateComponent()->GetMaxStamina();
		}
	}
}

void UPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Owner)
	{
		if (bUpdateSoul)
		{
			SoulAlpha += 1.0f / 60.0f;
			uint64 NewSoul = FMath::Lerp(CurrentSoul, Owner->GetSoul(), SoulAlpha);
			//CurrentSoul++;
			SoulText->SetText(FText::FromString(FString::FromInt(NewSoul)));

			if (SoulAlpha >= 1.0f)
			{
				SoulAlpha = 0.0f;
				bUpdateSoul = false;
				CurrentSoul = Owner->GetSoul();
				SoulText->SetText(FText::FromString(FString::FromInt(CurrentSoul)));
			}
		}
		if (bUpdateHealth)
		{
			HealthAlpha += 1.0f / 60.0f;

			float TargetPercent = Owner->GetStateComponent()->GetCurrentHealth() / Owner->GetStateComponent()->GetMaxHealth();

			float NewPercent = FMath::Lerp(CurrentHealth, TargetPercent, HealthAlpha);
			HealthBackBar->SetPercent(NewPercent);

			if (HealthAlpha >= 1.0f)
			{
				HealthAlpha = 0.0f;
				bUpdateHealth = false;
				CurrentHealth = Owner->GetStateComponent()->GetCurrentHealth() / Owner->GetStateComponent()->GetMaxHealth();
				HealthBackBar->SetPercent(CurrentHealth);
			}
		}
		if (bUpdateMana)
		{
			ManaAlpha += 1.0f / 60.0f;

			float TargetPercent = Owner->GetStateComponent()->GetCurrentMana() / Owner->GetStateComponent()->GetMaxMana();

			float NewPercent = FMath::Lerp(CurrentMana, TargetPercent, ManaAlpha);
			ManaBackBar->SetPercent(NewPercent);

			if (ManaAlpha >= 1.0f)
			{
				ManaAlpha = 0.0f;
				bUpdateMana = false;
				CurrentMana = Owner->GetStateComponent()->GetCurrentMana() / Owner->GetStateComponent()->GetMaxMana();
				ManaBackBar->SetPercent(CurrentMana);
			}
		}
		if (bUpdateStamina)
		{
			StaminaAlpha += 1.0f / 60.0f;
			
			float TargetPercent = Owner->GetStateComponent()->GetCurrentStamina() / Owner->GetStateComponent()->GetMaxStamina();

			float NewPercent = FMath::Lerp(CurrentStamina, TargetPercent, StaminaAlpha);
			StaminaBackBar->SetPercent(NewPercent);

			if (StaminaAlpha >= 1.0f)
			{
				StaminaAlpha = 0.0f;
				bUpdateStamina = false;
				CurrentStamina = Owner->GetStateComponent()->GetCurrentStamina() / Owner->GetStateComponent()->GetMaxStamina();
				StaminaBackBar->SetPercent(CurrentStamina);
			}
		}
	}
}

bool UPlayerWidget::InteractionEnabled()
{
	if (Owner)
	{
		return !Owner->GetCharacterMoving();
	}
	return false;
}

ESlateVisibility UPlayerWidget::IsActiveInGameMenu()
{	
	if(!Owner || !Owner->GetHUD()) return ESlateVisibility::Hidden;

	return Owner->GetHUD()->GetActiveMenu() != EActiveMenu::PlayerMenu ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible;
}

float UPlayerWidget::GetHealthPercent()
{
	float Percent = 0.f;
	if (!Owner) return Percent;

	Percent = Owner->GetStateComponent()->GetCurrentHealth() / Owner->GetStateComponent()->GetMaxHealth();

	return Percent;
}

float UPlayerWidget::GetManaPercent()
{
	float Percent = 0.f;
	if(!Owner) return Percent;

	Percent = Owner->GetStateComponent()->GetCurrentMana() / Owner->GetStateComponent()->GetMaxMana();

	return Percent; 
}

float UPlayerWidget::GetStaminaPercent()
{
	float Percent = 0.f;
	if (!Owner) return Percent;

	Percent = Owner->GetStateComponent()->GetCurrentStamina() / Owner->GetStateComponent()->GetMaxStamina();

	return Percent;
}

void UPlayerWidget::SetInteractionText(FText InText)
{
	InteractionText->SetText(InText);
}

ESlateVisibility UPlayerWidget::IsTwiceToOverlap()
{
	if(!Owner) return ESlateVisibility::Hidden;

	TArray<AActor*> OverlapActors;
	Owner->GetOverlappingActors(OverlapActors);
	if(OverlapActors.Num() > 1) return ESlateVisibility::SelfHitTestInvisible;
	else return ESlateVisibility::Hidden;
}

void UPlayerWidget::OnInteractionBox()
{
	PlayAnimation(InteractionVisible);
}

void UPlayerWidget::OffInteractionBox()
{
	PlayAnimation(InteractionHidden);
}

void UPlayerWidget::SwitchBossBox(bool bVisible)
{
	BossState_Box->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

float UPlayerWidget::CheckBossHpValue()
{
	float Percent = 0.f;
	if (!Owner || !Owner->GetTargetBoss()) return Percent;

	Percent = Owner->GetTargetBoss()->GetStateComponent()->GetCurrentHealth() / Owner->GetTargetBoss()->GetStateComponent()->GetMaxHealth();

	return Percent;
}

FText UPlayerWidget::UpdatedDecreaseHealth()
{
	if(!Owner || !Owner->GetTargetBoss()) return FText();

	int32 DecreaseValue =  Owner->GetTargetBoss()->GetStateComponent()->GetDecreaseHealth();
	if(DecreaseValue == 0) return FText();

	return FText::AsNumber(DecreaseValue);
}

void UPlayerWidget::RefreshItemViewList(FInventoryData Item)
{
	UItemViewWidget* Row = CreateWidget<UItemViewWidget>(this, ViewRowClass);
	if (Row)
	{
		Row->Setup(Item);
		ViewList->AddChild(Row);
	}
}

FText UPlayerWidget::UpdatedExpendableText()
{
	FText CurrentText = FText();

	if (Owner)
	{
		if(!Owner->GetInventoryComponent()) return CurrentText;

		if (Owner->GetExpendableData().IsSet())
		{
			CurrentText = FText::FromName(Owner->GetExpendableData().ItemData.ItemName);
		}
		else
		{
			CurrentText = FText();
		}
	}


	return CurrentText;
}

FText UPlayerWidget::UpdatedMagicText()
{
	FText CurrentText = FText();

	if (Owner)
	{
		if (!Owner->GetInventoryComponent()) return CurrentText;

		if (Owner->Get_E_SpellData().IsSet())
		{
			CurrentText = FText::FromName(Owner->Get_E_SpellData().ItemData.ItemName);
		}
		else
		{
			CurrentText = FText();
		}
	}


	return CurrentText;
}

void UPlayerWidget::PlayEventUI(FText Text, FLinearColor Tint)
{
	EventText->SetText(Text);
	EventText->SetColorAndOpacity(Tint);

	PlayAnimation(EventBlink);
}

void UPlayerWidget::CallMessageBox(FText Text)
{	
	MessageText->SetText(Text);
	PlayAnimation(MessageBlink);
}

void UPlayerWidget::BindPawn(APlayerCharacter* InPawn)
{
	if(!InPawn) return;

	Owner = InPawn;

}

bool UPlayerWidget::CheckLSlotEnabled()
{
	if(!Owner) return false;

	if(Owner->GetHoldType() == EEquipHand::None || Owner->GetHoldType() == EEquipHand::Left) return true;
	
	return false;
}

bool UPlayerWidget::CheckRSlotEnabled()
{
	if (!Owner) return false;

	if (Owner->GetHoldType() == EEquipHand::None || Owner->GetHoldType() == EEquipHand::Right) return true;

	return false;
}

bool UPlayerWidget::CheckSpellSlotEnabled()
{
	if(!Owner || !Owner->Get_E_SpellData().IsSet()) return false;

	if (Owner->Get_E_SpellData().SpellData.SpellType == ESpellType::AttackSorcery || Owner->Get_E_SpellData().SpellData.SpellType == ESpellType::AssistSorcery)
	{
		if (Owner->GetMainWeapon().WeaponData.WeaponType == EWeaponType::Wand)
		{
			return true;
		}
	}
	if (Owner->Get_E_SpellData().SpellData.SpellType == ESpellType::AttackIncantation || Owner->Get_E_SpellData().SpellData.SpellType == ESpellType::AssistIncantation)

	{
		if (Owner->GetSecondaryWeapon().WeaponData.WeaponType == EWeaponType::Talisman)
		{
			return true;
		}
	}
	return false;
}

void UPlayerWidget::UpdatedSlot(EItemType SlotType, EEquipHand HandType, FInventoryData ItemData, bool bAnim)
{
	if (SlotType == EItemType::Weapon)
	{
		if (HandType == EEquipHand::None) return;
		else if (HandType == EEquipHand::Left)
		{
			ItemData.IsSet() ? L_WeaponSlot->AddSlotItem(ItemData) : L_WeaponSlot->RemoveSlotItem();
			if (bAnim) L_WeaponSlot->PlaySlotAnim();
		}
		else if (HandType == EEquipHand::Right)
		{
			ItemData.IsSet() ? R_WeaponSlot->AddSlotItem(ItemData) : R_WeaponSlot->RemoveSlotItem();
			if (bAnim) R_WeaponSlot->PlaySlotAnim();
		}
	}
	if (SlotType == EItemType::Expendable)
	{
		ItemData.IsSet() ? ExpendableSlot->AddSlotItem(ItemData) : ExpendableSlot->RemoveSlotItem();
		if (bAnim) ExpendableSlot->PlaySlotAnim();
	}
	if (SlotType == EItemType::Spell)
	{
		ItemData.IsSet() ? SpellSlot->AddSlotItem(ItemData) : SpellSlot->RemoveSlotItem();
		if (bAnim) SpellSlot->PlaySlotAnim();
	}
	if (SlotType == EItemType::Arrow)
	{
		if(ItemData.IsSet())
		{
			if(ItemData.EquipIndex == 0) FirstArrowSlot->AddSlotItem(ItemData);
			else if(ItemData.EquipIndex == 1) SecondArrowSlot->AddSlotItem(ItemData);
		}
		else
		{
			if(!Owner->GetInventoryComponent()->ArrowEquip.Find(0)) FirstArrowSlot->RemoveSlotItem();
			else if(!Owner->GetInventoryComponent()->ArrowEquip.Find(1)) SecondArrowSlot->RemoveSlotItem();
			//ArrowSlot->RemoveSlotItem();
		}
		//if (bAnim) ArrowSlot->PlaySlotAnim();
	}

}

void UPlayerWidget::UpdatedSoulText(int32 InSoul, bool bAnim)
{
	if(!Owner) return;
	if (bAnim)
	{
		FString OutputText = FString::FromInt(InSoul);
		
		AddSoulText->SetText(FText::FromString(OutputText));
		PlayAnimation(AddSoul);
	}
	else
	{
		CurrentSoul = Owner->GetSoul();
		SoulText->SetText(FText::FromString(FString::FromInt(CurrentSoul)));
	}
}

void UPlayerWidget::UpdateHealthText(bool bAnim)
{
	if (!Owner) return;
	if (bAnim)
	{
		bUpdateHealth = true;
	}
	else
	{
		CurrentHealth = Owner->GetStateComponent()->GetCurrentHealth() / Owner->GetStateComponent()->GetMaxHealth();
		HealthBackBar->SetPercent(CurrentHealth);
	}
}

void UPlayerWidget::UpdateManaText(bool bAnim)
{
	if (!Owner) return;
	if (bAnim)
	{
		bUpdateMana = true;

	}
	else
	{
		CurrentMana = Owner->GetStateComponent()->GetCurrentMana() / Owner->GetStateComponent()->GetMaxMana();
		ManaBackBar->SetPercent(CurrentMana);
	}
}

void UPlayerWidget::UpdateStaminaText(bool bAnim)
{
	if (!Owner) return;
	if (bAnim)
	{
		bUpdateStamina = true;

	}
	else
	{
		CurrentStamina = Owner->GetStateComponent()->GetCurrentStamina() / Owner->GetStateComponent()->GetMaxStamina();
		StaminaBackBar->SetPercent(CurrentStamina);
	}
}

void UPlayerWidget::PlayUpdatedSoul()
{
	bUpdateSoul = true;
}

void UPlayerWidget::SetTimerVisibility(bool bActivate)
{
	bActivate ? PlayAnimation(TimerOn) : PlayAnimation(TimerOff);
}

void UPlayerWidget::UpdateTimer(float InTimer)
{
	TimerText->SetVisibility(ESlateVisibility::Visible);
	FString TimeText;

	int32 Minute = (int32)InTimer / 60;
	int32 Second = (int32)InTimer - (Minute * 60);

	TimeText = Minute < 10 ? FString::FromInt(0) + FString::FromInt(Minute) : FString::FromInt(Minute);
	TimeText += TEXT(" : ");
	TimeText += Second < 10 ? FString::FromInt(0) + FString::FromInt(Second) : FString::FromInt(Second);

	//TimeText = FString::FromInt((int32)InTimer);

	TimerText->SetText(FText::FromString(TimeText));
}

