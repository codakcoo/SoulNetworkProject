// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Inventroy/InvenType_InventorySlotWidget.h"
#include "Widget/Slot/InventoryType_SlotWidget.h"
#include "Widget/Slot/ActionMenuWidget.h"
#include "Widget/Slot/ConfirmMenuWidget.h"
#include "Widget/Slot/GradeSlotWidget.h"

#include "Classes/InventoryComponent.h"

#include "SoulNetworkProject/PlayerHUD.h"
#include "SoulNetworkProject/ItemState.h"
#include "SoulNetworkProject/ItemData.h"


#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/WrapBox.h"
#include "Delegates/DelegateSignatureImpl.inl"

UInvenType_InventorySlotWidget::UInvenType_InventorySlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> GradeRowClass_C(TEXT("WidgetBlueprint'/Game/Widget/Item/Inventory/InvenGradeSlot_BP.InvenGradeSlot_BP_C'"));
	if (GradeRowClass_C.Succeeded())
	{
		GradeRowClass = GradeRowClass_C.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> ActionWidgetClass_C(TEXT("WidgetBlueprint'/Game/Widget/Item/Action/ActionMenu_BP.ActionMenu_BP_C'"));
	if (ActionWidgetClass_C.Succeeded())
	{
		ActionWidgetClass = ActionWidgetClass_C.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> ConfirmWidgetClass_C(TEXT("WidgetBlueprint'/Game/Widget/Item/Action/CofirmMenu_BP.CofirmMenu_BP_C'"));
	if (ConfirmWidgetClass_C.Succeeded())
	{
		ConfirmWidgetClass = ConfirmWidgetClass_C.Class;
	}
}

bool UInvenType_InventorySlotWidget::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	if (StateSelectBox)
	{
		StateSelectBox->bIsEnabledDelegate.BindUFunction(this, "CheckStateBox");
	}

	if (WeaponButton)
	{
		WeaponButton->OnClicked.AddDynamic(this, &UInvenType_InventorySlotWidget::OnWeaponClicked);
		WeaponButton->bIsEnabledDelegate.BindUFunction(this, "CheckWeapon");
	}
	if (HelmetButton)
	{
	HelmetButton->OnClicked.AddDynamic(this, &UInvenType_InventorySlotWidget::OnHelmetClicked);
	HelmetButton->bIsEnabledDelegate.BindUFunction(this, "CheckHelmet");

	}
	if (ArmorButton)
	{
		ArmorButton->OnClicked.AddDynamic(this, &UInvenType_InventorySlotWidget::OnArmorClicked);
		ArmorButton->bIsEnabledDelegate.BindUFunction(this, "CheckArmor");
	}
	if (PantButton)
	{
		PantButton->OnClicked.AddDynamic(this, &UInvenType_InventorySlotWidget::OnPantClicked);
		PantButton->bIsEnabledDelegate.BindUFunction(this, "CheckPant");
	}
	if (BootsButton)
	{
		BootsButton->OnClicked.AddDynamic(this, &UInvenType_InventorySlotWidget::OnBootsClicked);
		BootsButton->bIsEnabledDelegate.BindUFunction(this, "CheckBoots");
	}
	if (ExpendableButton)
	{
		ExpendableButton->OnClicked.AddDynamic(this, &UInvenType_InventorySlotWidget::OnExpendableClicked);
		ExpendableButton->bIsEnabledDelegate.BindUFunction(this, "CheckExpendable");
	}
	if (SpellButton)
	{
		SpellButton->OnClicked.AddDynamic(this, &UInvenType_InventorySlotWidget::OnSpellClicked);
		SpellButton->bIsEnabledDelegate.BindUFunction(this, "CheckSpell");
	}
	if (ArrowButton)
	{
		ArrowButton->OnClicked.AddDynamic(this, &UInvenType_InventorySlotWidget::OnArrowClicked);
		ArrowButton->bIsEnabledDelegate.BindUFunction(this, "CheckArrow");
	}
	if (EnhancementButton)
	{
		EnhancementButton->OnClicked.AddDynamic(this, &UInvenType_InventorySlotWidget::OnEnhancementClicked);
		EnhancementButton->bIsEnabledDelegate.BindUFunction(this, "CheckEnhancement");
	}

	if (WidgetText)
	{
		WidgetText->TextDelegate.BindUFunction(this, "UpdateWidgetText");
	}

	if (ApplyBox)
	{
		ApplyBox->VisibilityDelegate.BindUFunction(this, "UpdateApplyBox");
	}
	if (DescriptionText)
	{
		DescriptionText->TextDelegate.BindUFunction(this, "UpdateDescriptionText");
	}

	return true;
}

void UInvenType_InventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//ActionWidget = CreateWidget<UActionMenuWidget>(this, ActionWidgetClass);
	//ConfirmWidget = CreateWidget<UConfirmMenuWidget>(this, ConfirmWidgetClass);

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UInvenType_InventorySlotWidget::RefreshInventory);
	}

	RefreshInventory();
}

void UInvenType_InventorySlotWidget::RefreshInventory()
{
	ChangeInventorySlot(WidgetItemType);
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
		DiabledInheritedItemSlots();
	}
	
}
////////////////////////////////////////////////////////////////

void UInvenType_InventorySlotWidget::SwitchingMenu(bool bForward)
{
	if(InventoryType == EInventoryType::Action) return;

	if (bForward)
	{
		//MenuIndex++;
		uint8 IncreaseValue = static_cast<uint8>(WidgetItemType) + 1;
		WidgetItemType = static_cast<EItemType>(IncreaseValue);
		if((uint8)WidgetItemType > static_cast<uint8>(EItemType::Enhancement)) WidgetItemType = EItemType::Expendable;
	}
	else
	{
		//MenuIndex--;
		uint8 IncreaseValue = static_cast<uint8>(WidgetItemType) - 1;
		WidgetItemType = static_cast<EItemType>(IncreaseValue);
		if((uint8)WidgetItemType < static_cast<uint8>(EItemType::Expendable)) WidgetItemType = EItemType::Enhancement;
	}

	//WidgetItemType = static_cast<EItemType>(MenuIndex);
	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}

void UInvenType_InventorySlotWidget::ActivateInventory()
{
	InventoryType = EInventoryType::None;
	ConfirmType = EConfirmType::None;
	ReactionType = EReactionType::Single;
	if(WidgetItemType == EItemType::All)
	{
		if(SelectSlots.Num() > 0)
		{
			WidgetItemType = SelectSlots[0]->SlotData.ItemData.ItemType;
		}
	}
	for (auto& ItemSlot : SelectSlots)
	{
		ItemSlot->SetSelect(false);
	}
	SelectSlots.Empty();
	UpdatedItemSlots(true);

	if(ActionWidget)
	{
		ActionWidget->UpdateActionMenu();
		ActionWidget = nullptr;
	}
	if(ConfirmWidget)
	{
		ConfirmWidget->AllRemoveWidget();
		ConfirmWidget = nullptr;
	}

	RefreshInventory();
}

void UInvenType_InventorySlotWidget::ActivateAction()
{
	if(SelectSlots.Num() == 0) return;

	if(BeforeType == EItemType::All) BeforeType = WidgetItemType;
	WidgetItemType = EItemType::All;
	InventoryType = EInventoryType::Action;

	//for (int8 i = SelectSlots.Num(); i == 0; i--)
	//{
	//	SelectSlots.Pop();
	//}
	for (auto& Row : GradeBox->GetAllChildren())
	{
		if (UGradeSlotWidget* GradeRow = Cast<UGradeSlotWidget>(Row))
		{
			GradeRow->SetSlotEnabled(false, SelectSlots[0]);
		}
	}

	if(ActionWidget) ActionWidget->VisibilityActionMenu();
	else
	{
		ActionWidget = CreateWidget<UActionMenuWidget>(this, ActionWidgetClass);
		
		ActionWidget->SetupData(this);
		ActionWidget->SetDescriptorText(TEXT("Choose the amount."));
		ActionWidget->UpdateActionMenu(SelectSlots[0]);
	}

	if(ConfirmWidget) ConfirmWidget->AllRemoveWidget();
}
void UInvenType_InventorySlotWidget::ActivateConfirm(EConfirmType InConfirmType, EReactionType InReactionType)
{
	if (SelectSlots.Num() == 0) return;
	InventoryType = EInventoryType::Confirm;

	ConfirmType = InConfirmType;
	ReactionType = InReactionType;

	if(ActionWidget) ActionWidget->HiddenActionMenu();
	ConfirmWidget = CreateWidget<UPopupWidget>(this, ConfirmWidgetClass);
	if (ConfirmWidget)
	{
		ConfirmWidget->SetupData(this);
		ConfirmType == EConfirmType::Drop ? 
			ConfirmWidget->SetDescriptorText(TEXT("Are you sure you want to drop the item")) :
			ConfirmWidget->SetDescriptorText(TEXT("Are you sure you want to destroy the item"));
	}
}
void UInvenType_InventorySlotWidget::ActivateSelected(EConfirmType InConfirmType, EReactionType InReactionType)
{
	if(SelectSlots.Num() == 0) return;
	InventoryType = EInventoryType::Seleced;
	ConfirmType = InConfirmType;
	ReactionType = InReactionType;
	WidgetItemType = SelectSlots[0]->SlotData.ItemData.ItemType;

	if (SelectSlots.Num() == 5)
	{
		SelectSlots.Last()->SetSelect(false);
		SelectSlots.Pop();
	}

	if (ActionWidget && InReactionType == EReactionType::Multi)
	{
		ActionWidget->AllRemoveWidget();
		ActionWidget = nullptr;
	}
	if (ConfirmWidget)
	{
		ConfirmWidget->AllRemoveWidget();
		ConfirmWidget = nullptr;
	}
	UpdatedItemSlots(true);
	DiabledInheritedItemSlots();

}
///////////////////////////////////////////////////////////

FText UInvenType_InventorySlotWidget::UpdateWidgetText()
{
	FString GetTypeName;
	GetTypeName = EnumToString(WidgetItemType);

	return FText::FromString(GetTypeName);
}
ESlateVisibility UInvenType_InventorySlotWidget::UpdateApplyBox()
{
	return InventoryType == EInventoryType::Seleced ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
}
FText UInvenType_InventorySlotWidget::UpdateDescriptionText()
{
	FString Description;

	switch (InventoryType)
	{
		case EInventoryType::None:
		case EInventoryType::Action:
		{
			Description = "Please select an item to manipulate";
		}
		break;
		case EInventoryType::Seleced:
		{
			if(ConfirmType == EConfirmType::Drop)
			{
				Description = "Select items to put down in their place all at once (" + FString::FromInt(SelectSlots.Num()) + "/5)";
			}
			else if(ConfirmType == EConfirmType::Destroy)
			{
				Description = "Please select the items to be destroyed at once (" + FString::FromInt(SelectSlots.Num()) + "/5)";
			}
		}
		default:
			break;
	}

	return FText::FromString(Description);
}
////////////////////////////////////////////////////////

void UInvenType_InventorySlotWidget::AllCheck()
{
	//CheckAll();
	WidgetItemType = WidgetItemType == EItemType::All ? EItemType::Expendable : WidgetItemType;

	CheckWeapon();
	CheckHelmet();
	CheckArmor();
	CheckPant();
	CheckBoots();
	CheckExpendable();
	CheckSpell();
	CheckArrow();
	CheckEnhancement();
}
void UInvenType_InventorySlotWidget::UpdatedItemSlots(bool bEnabledOtherSlot, USlotWidget* ItemSlot)
{
	if(!bEnabledOtherSlot && !ItemSlot) return;

	for (auto& Row : GradeBox->GetAllChildren())
	{
		if (UGradeSlotWidget* GradeRow = Cast<UGradeSlotWidget>(Row))
		{
			GradeRow->SetSlotEnabled(bEnabledOtherSlot, !bEnabledOtherSlot ? ItemSlot : nullptr);
		}
	}

	//if(ItemSlot) 
	//{
	//	if(!ActionWidget) ActionWidget = CreateWidget<UActionMenuWidget>(this, ActionWidgetClass);
	//}
	//if (ActionWidget)
	//{
	//	ActionWidget->SetupData(this);
	//	ActionWidget->UpdateActionMenu(!bEnabledOtherSlot ? ItemSlot : nullptr);
	//}
	//StateSelectBox->SetIsEnabled(bEnabledOtherSlot);
}
void UInvenType_InventorySlotWidget::DiabledInheritedItemSlots()
{
	for (auto& Row : GradeBox->GetAllChildren())
	{
		if (UGradeSlotWidget* GradeRow = Cast<UGradeSlotWidget>(Row))
		{
			GradeRow->DiabledInheritedItem();
		}
	}
}
void UInvenType_InventorySlotWidget::UpdatedSelectedItemSlots()
{
	for (auto& SelectSlot : SelectSlots)
	{
		for (auto& Row : GradeBox->GetAllChildren())
		{
			if (UGradeSlotWidget* GradeRow = Cast<UGradeSlotWidget>(Row))
			{
				GradeRow->UpdatedSelectedItem(SelectSlot->SlotData);
			}
		}
	}
}
void UInvenType_InventorySlotWidget::CheckStackCount()
{
	if (SelectSlots.Num() >= 5)
	{
		ActivateConfirm(ConfirmType, ReactionType);
	}
	else if (SelectSlots.Num() == 0)
	{
		//SelectSlots.Empty();
		//InventoryType = EInventoryType::None;
		ActivateInventory();
	}
}
////////////////////////////////////////////////////////////

void UInvenType_InventorySlotWidget::OnReaction(USlotWidget* ItemSlot)
{
	if(!ItemSlot || !ItemSlot->SlotData.IsSet()) return;

	ItemSlot->SetSelect(true);
	SelectSlots.Add(ItemSlot);
	CheckStackCount();

	if (InventoryType == EInventoryType::None)
	{
		ActivateAction();
	}
}
/////////////////////////////////////

void UInvenType_InventorySlotWidget::OffReaction(USlotWidget* ItemSlot)
{
	if (!ItemSlot || !ItemSlot->SlotData.IsSet()) return;

	if (InventoryType == EInventoryType::Action)
	{
		//for (auto& Row : GradeBox->GetAllChildren())
		//{
		//	if (UGradeSlotWidget* GradeRow = Cast<UGradeSlotWidget>(Row))
		//	{
		//		GradeRow->SetSlotEnabled(true);
		//	}
		//}
		//InventoryType = EInventoryType::None;
		//ActionWidget->UpdateActionMenu(nullptr);
		//ActionWidget = nullptr;
		//
		//UpdatedItemSlots(true);
		ActivateInventory();

		return;
	}


	ItemSlot->SetSelect(false);
	SelectSlots.RemoveSingle(ItemSlot);
	CheckStackCount();
}
void UInvenType_InventorySlotWidget::UpdatedWidgetText(USlotWidget* ItemSlot)
{
	UpdatedWidget(ItemSlot);
}


void UInvenType_InventorySlotWidget::ActiveDropMenu()
{
	ActivateConfirm(EConfirmType::Drop, EReactionType::Single);
}
void UInvenType_InventorySlotWidget::ActiveDestroyMenu()
{
	ActivateConfirm(EConfirmType::Destroy, EReactionType::Single);
}
//////////////////////////


void UInvenType_InventorySlotWidget::SetDropSelectMode()
{
	ActivateSelected(EConfirmType::Drop, EReactionType::Multi);
}
void UInvenType_InventorySlotWidget::SetDestroySelectMode()
{
	ActivateSelected(EConfirmType::Destroy, EReactionType::Multi);
}
////////////////////

//Use Interface//
void UInvenType_InventorySlotWidget::UseReaction()
{
	if (SelectSlots.Num() == 0 || !SelectSlots[0]->SlotData.IsSet()) return;

	InventoryComponent->UseItem(SelectSlots[0]->SlotData);
	ActivateAction();
	
	if (APlayerHUD* HUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
	{
		HUD->OnOffInGame();
	}
}
////////////////

//Slate Interface//
void UInvenType_InventorySlotWidget::ConfirmPopup()
{
	if (InventoryType == EInventoryType::Action)
	{
		if (SelectSlots.Num() == 0 || !SelectSlots[0]->SlotData.IsSet()) return;
		if (ActionWidget)
		{
			ActionWidget->GetActionType() == EActionType::Drop ? 
				InventoryComponent->DropItem(SelectSlots[0]->SlotData, ActionWidget->GetCurrentCount()) :
				InventoryComponent->DestroyItem(SelectSlots[0]->SlotData, ActionWidget->GetCurrentCount());

			ActivateInventory();

			UpdatedItemSlots(true);
		}
	}
	else if (InventoryType == EInventoryType::Confirm)
	{
		if (SelectSlots.Num() == 0 || !SelectSlots[0]->SlotData.IsSet()) return;

		TArray<FInventoryData> SlotDatas;
		for (auto& ItemSlot : SelectSlots)
		{
			SlotDatas.Add(ItemSlot->SlotData);
		}
		
		ConfirmType == EConfirmType::Drop ? 
			InventoryComponent->DropAtOnceItem(SlotDatas) :
			InventoryComponent->DestroyAtOnceItem(SlotDatas);

		ActivateInventory();
	}
}
void UInvenType_InventorySlotWidget::CancelPopup()
{
	if (SelectSlots.Num() == 0) return;

	if (InventoryType == EInventoryType::Action)
	{
		//for (auto& Row : GradeBox->GetAllChildren())
		//{
		//	if (UGradeSlotWidget* GradeRow = Cast<UGradeSlotWidget>(Row))
		//	{
		//		GradeRow->SetSlotEnabled(true);
		//	}
		//}
		//InventoryType = EInventoryType::None;
		//
		//ActionWidget->UpdateActionMenu(nullptr);
		//ActionWidget = nullptr;
		//
		//SelectSlots[0]->SetSelect(false);
		//SelectSlots.Empty();
		//CheckStackCount();
		ActivateInventory();

		//SelectSlots[0]->SetSelect(false);
		//SelectSlots.Empty();
		//CheckStackCount();
	}
	else
	{
		if (ReactionType == EReactionType::Single)
		{
			ActivateAction();
		}
		else if (ReactionType == EReactionType::Multi)
		{

			ActivateSelected(ConfirmType, ReactionType);

			CheckStackCount();
		}
	}
}
void UInvenType_InventorySlotWidget::BackgroundPopup()
{
	CancelPopup();
}
//////////////////////////////////


void UInvenType_InventorySlotWidget::OnWeaponClicked()
{
	WidgetItemType = EItemType::Weapon;

	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}

void UInvenType_InventorySlotWidget::OnHelmetClicked()
{
	WidgetItemType = EItemType::Helmet;

	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}

void UInvenType_InventorySlotWidget::OnArmorClicked()
{
	WidgetItemType = EItemType::Armor;

	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}

void UInvenType_InventorySlotWidget::OnPantClicked()
{
	WidgetItemType = EItemType::Pant;

	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}

void UInvenType_InventorySlotWidget::OnBootsClicked()
{
	WidgetItemType = EItemType::Boots;

	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}

void UInvenType_InventorySlotWidget::OnExpendableClicked()
{
	WidgetItemType = EItemType::Expendable;

	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}

void UInvenType_InventorySlotWidget::OnSpellClicked()
{
	WidgetItemType = EItemType::Spell;

	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}

void UInvenType_InventorySlotWidget::OnArrowClicked()
{
	WidgetItemType = EItemType::Arrow;

	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}
void UInvenType_InventorySlotWidget::OnEnhancementClicked()
{
	WidgetItemType = EItemType::Enhancement;

	RefreshInventory();
	if (InventoryType == EInventoryType::Seleced)
	{
		UpdatedSelectedItemSlots();
	}
}
/////////////////////////////
