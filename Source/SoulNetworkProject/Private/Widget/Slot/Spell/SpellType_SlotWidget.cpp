// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Spell/SpellType_SlotWidget.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "Interface/SpellEquipmentInterface.h"

bool USpellType_SlotWidget::Initialize()
{
	bool bSuccess =  Super::Initialize();
	if(!bSuccess) return false;

	if(SelectImage) SelectImage->VisibilityDelegate.BindUFunction(this, "bSelectedEnabled");

	return true;
}

void USpellType_SlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USpellType_SlotWidget::OnClicked()
{
	if (OwnerHUD->GetSpellSlot()->GetClass()->ImplementsInterface(USpellEquipmentInterface::StaticClass()))
	{
		Cast<ISpellEquipmentInterface>(OwnerHUD->GetSpellSlot())->OnSelectMenu();
	}
}

void USpellType_SlotWidget::OnHovered()
{
	Super::OnHovered();
	if (OwnerHUD->GetSpellSlot()->GetClass()->ImplementsInterface(USpellEquipmentInterface::StaticClass()))
	{
		Cast<ISpellEquipmentInterface>(OwnerHUD->GetSpellSlot())->UpdateEquipWidgetText(this);
	}
}