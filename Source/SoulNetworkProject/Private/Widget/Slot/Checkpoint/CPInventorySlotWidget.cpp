// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Checkpoint/CPInventorySlotWidget.h"
#include "Widget/Slot/InventoryType_SlotWidget.h"
#include "Widget/Slot/ActionMenuWidget.h"
#include "Widget/Slot/Popup/StoragePopupWidget.h"
#include "Widget/Slot/GradeSlotWidget.h"
#include "Classes/InventoryComponent.h"
#include "SaveGame/SaveCharacterState.h"
#include "Interface/SaveInterface.h"
#include "SoulNetworkProject/PlayerHUD.h"
#include "SoulNetworkProject/ItemState.h"

#include "SoulNetworkProject/ItemData.h"


#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/WrapBox.h"
#include "Delegates/DelegateSignatureImpl.inl"

UCPInventorySlotWidget::UCPInventorySlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> GradeRowClass_C(TEXT("WidgetBlueprint'/Game/Widget/Game/CheckPoint/Storage/StorageGradeSlot_BP.StorageGradeSlot_BP_C'"));
	if (GradeRowClass_C.Succeeded())
	{
		GradeRowClass = GradeRowClass_C.Class;
	}
}

bool UCPInventorySlotWidget::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (StateSelectBox)
	{
		StateSelectBox->bIsEnabledDelegate.BindUFunction(this, "CheckStateBox");
	}

	if (WeaponButton)
	{
		WeaponButton->OnClicked.AddDynamic(this, &UCPInventorySlotWidget::OnWeaponClicked);
		WeaponButton->bIsEnabledDelegate.BindUFunction(this, "CheckWeapon");
	}
	if (HelmetButton)
	{
		HelmetButton->OnClicked.AddDynamic(this, &UCPInventorySlotWidget::OnHelmetClicked);
		HelmetButton->bIsEnabledDelegate.BindUFunction(this, "CheckHelmet");

	}
	if (ArmorButton)
	{
		ArmorButton->OnClicked.AddDynamic(this, &UCPInventorySlotWidget::OnArmorClicked);
		ArmorButton->bIsEnabledDelegate.BindUFunction(this, "CheckArmor");
	}
	if (PantButton)
	{
		PantButton->OnClicked.AddDynamic(this, &UCPInventorySlotWidget::OnPantClicked);
		PantButton->bIsEnabledDelegate.BindUFunction(this, "CheckPant");
	}
	if (BootsButton)
	{
		BootsButton->OnClicked.AddDynamic(this, &UCPInventorySlotWidget::OnBootsClicked);
		BootsButton->bIsEnabledDelegate.BindUFunction(this, "CheckBoots");
	}
	if (ExpendableButton)
	{
		ExpendableButton->OnClicked.AddDynamic(this, &UCPInventorySlotWidget::OnExpendableClicked);
		ExpendableButton->bIsEnabledDelegate.BindUFunction(this, "CheckExpendable");
	}
	if (SpellButton)
	{
		SpellButton->OnClicked.AddDynamic(this, &UCPInventorySlotWidget::OnSpellClicked);
		SpellButton->bIsEnabledDelegate.BindUFunction(this, "CheckSpell");
	}
	if (ArrowButton)
	{
		ArrowButton->OnClicked.AddDynamic(this, &UCPInventorySlotWidget::OnArrowClicked);
		ArrowButton->bIsEnabledDelegate.BindUFunction(this, "CheckArrow");
	}
	if (EnhancementButton)
	{
		EnhancementButton->OnClicked.AddDynamic(this, &UCPInventorySlotWidget::OnEnhancementClicked);
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
	//if (DescriptionText)
	//{
	//	DescriptionText->TextDelegate.BindUFunction(this, "UpdateDescriptionText");
	//}

	if (GradeBox) GradeBox->bIsEnabledDelegate.BindUFunction(this, "UpdateGradeBox");
	if(StorageGradeBox) StorageGradeBox->bIsEnabledDelegate.BindUFunction(this, "UpdateStorageGradeBox");

	return true;
}

void UCPInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//ActionWidget = CreateWidget<UActionMenuWidget>(this, ActionWidgetClass);
	//ConfirmWidget = CreateWidget<UConfirmMenuWidget>(this, ConfirmWidgetClass);

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UCPInventorySlotWidget::RefreshInventory);
	}

	RefreshInventory();
}

void UCPInventorySlotWidget::UpdatedItemSlot(USlotWidget* ItemSlot)
{
		if (ItemSlot && ItemSlot->SlotData.IsSet())
		{
			if (ItemSlot->SlotData.ItemData.ItemType == EItemType::Expendable ||
				ItemSlot->SlotData.ItemData.ItemType == EItemType::Arrow)
			{
				if(ItemSlot->SlotData.ItemCount <= 0) return;

				if(ItemSlot->SlotData.ItemCount == 1) CallApplyDetailData(ItemSlot->SlotData);
				else
				{
					SelectSlot = ItemSlot;
					//ActiveConfirmMenu();

					if (SelectSlot)
					{
						if (ConfirmWidgetClass)
						{
							if (!ConfirmWidget) ConfirmWidget = CreateWidget<UStoragePopupWidget>(this, ConfirmWidgetClass);

							InventoryType = EInventoryType_CP::Confirm;
							ConfirmWidget->SetupData(this);
							ConfirmWidget->SetSlotData(ItemSlot);
						}
					}
				}
			}
			else
			{
				CallApplyDetailData(ItemSlot->SlotData);
			}
		}
	//}
}

void UCPInventorySlotWidget::UpdateItemText(USlotWidget* ItemSlot) 
{
}

void UCPInventorySlotWidget::ConfirmPopup()
{
	if (ConfirmWidget && SelectSlot)
	{
		if (ConfirmWidget->GetCurrnetCount() > 0)
		{
			FInventoryData SlotData = SelectSlot->SlotData;
			SlotData.ItemCount = ConfirmWidget->GetCurrnetCount();
			CallApplyDetailData(SlotData);
			RefreshInventory();

			InventoryType = EInventoryType_CP::None;
			ConfirmWidget->AllRemoveWidget();
			ConfirmWidget = nullptr;
			SelectSlot = nullptr;
		}
	}
}

void UCPInventorySlotWidget::CancelPopup()
{
	if (ConfirmWidget)
	{
		InventoryType = EInventoryType_CP::None;
		SelectSlot = nullptr;
		ConfirmWidget->AllRemoveWidget();//RemoveFromViewport();
		ConfirmWidget = nullptr;

	}
}

void UCPInventorySlotWidget::BackgroundPopup()
{
	CancelPopup();
}

//아이템을 먹거나 버리면 인벤토리를 리프레쉬하게 AddDynamic시킴//
void UCPInventorySlotWidget::RefreshInventory()
{
	ChangeInventorySlot(WidgetItemType);
	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		USaveCharacterState* SaveData = Cast<ISaveInterface>(GetGameInstance())->GetCurrentData();
		if (!SaveData) return;
		PlayAnimation(StorageInvenBlink);


		if (WidgetItemType == EItemType::Expendable)
		{
			uint8 disableCount = 0;
			for (auto& rowWidget : GradeBox->GetAllChildren())
			{
				if (UGradeSlotWidget* currGradeSlot = Cast<UGradeSlotWidget>(rowWidget))
				{
					for (auto& columnWidget : currGradeSlot->GetSlotArr())
					{
						if (USlotWidget* currSlot = Cast<USlotWidget>(columnWidget))
						{

							if (currSlot->SlotData.IsSet())
							{
								if (currSlot->SlotData.ItemData.ItemName == TEXT("HealthPotion") ||
									currSlot->SlotData.ItemData.ItemName == TEXT("ManaPotion"))
								{
									currSlot->SetIsEnabled(false);
									++disableCount;

								}

								if (disableCount == 2) break;
							}
						}
					}
				}
			}
		}


		//Storage GradeBox//
		StorageGradeBox->ClearChildren();

		TArray<FInventoryData*> CategoryItems;

		if(SaveData->StorageData.Num() == 0) return;
		for (auto& Item : SaveData->StorageData)
		{
			if ((uint8)Item.ItemData.ItemType == (uint8)WidgetItemType)
			{
				CategoryItems.Add(&Item);

			}
		}
		if (CategoryItems.Num() == 0) return;

		uint8 RowIndex = 0;
		uint8 SlotIndex = 0;

		const UEnum* EnumObject = GetEnumObject(WidgetItemType);
		if (!EnumObject) return;


		UGradeSlotWidget* CreateCurrentSlot = CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
		for (uint8 i = 0; i < EnumObject->GetMaxEnumValue(); ++i)
		{
			CategoryItems = CallbackInventorySlot(WidgetItemType, i, CategoryItems, StorageGradeBox, CreateCurrentSlot, RowIndex, SlotIndex);

			if (CategoryItems.Num() == 0) break;
		}

	}
}
////////////////////////////////////////////////////////////////

//인벤토리 z,x로 인덱스 변경으로 해당슬롯인덱스로 아이템슬롯으로 바꿈//
void UCPInventorySlotWidget::SwitchingMenu(bool bForward)
{
	if (InventoryType == EInventoryType_CP::Confirm) return;

	if (bForward)
	{
		//MenuIndex++;
		uint8 IncreaseValue = static_cast<uint8>(WidgetItemType) + 1;
		WidgetItemType = static_cast<EItemType>(IncreaseValue);
		if ((uint8)WidgetItemType > static_cast<uint8>(EItemType::Enhancement)) WidgetItemType = EItemType::Expendable;
	}
	else
	{
		//MenuIndex--;
		uint8 IncreaseValue = static_cast<uint8>(WidgetItemType) - 1;
		WidgetItemType = static_cast<EItemType>(IncreaseValue);
		if ((uint8)WidgetItemType < static_cast<uint8>(EItemType::Expendable)) WidgetItemType = EItemType::Enhancement;
	}

	RefreshInventory();
}
///////////////////////////////////////////////////////////

void UCPInventorySlotWidget::SetupContentMenu()
{
	bLeft = true;
	InventoryType = EInventoryType_CP::None;
	WidgetItemType = EItemType::Expendable;
	RefreshInventory();

	//PlayAnimation(Blink);
}

void UCPInventorySlotWidget::SwitchActiveMenu()
{
	bLeft = bLeft ? false : true;
}

//위젯 텍스트를 해당 active중인 아이템타입에 맞게 변경함//
FText UCPInventorySlotWidget::UpdateWidgetText()
{
	FString GetTypeName;
	GetTypeName = EnumToString(WidgetItemType);

	return FText::FromString(GetTypeName);
}
ESlateVisibility UCPInventorySlotWidget::UpdateApplyBox()
{
	return ESlateVisibility::SelfHitTestInvisible;//InventoryType == EInventoryType::Seleced ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
}
FText UCPInventorySlotWidget::UpdateDescriptionText()
{
	FString GetTypeName;
	GetTypeName = EnumToString(WidgetItemType);

	return FText::FromString(GetTypeName);
}
////////////////////////////////////////////////////////

void UCPInventorySlotWidget::CallApplyDetailData(FInventoryData InData)
{
	ISaveInterface* SaverInterface = Cast<ISaveInterface>(GetGameInstance());
	if (SaverInterface && InventoryComponent)
	{
		if (InData.IsSet())
		{
			FInventoryData InvenData = InData;
			FInventoryData StorageData = InData;


			if (bLeft)
			{
				InventoryComponent->RemoveItem(InvenData);

				SaverInterface->SaveInvenWithStorage(InventoryComponent->PermuteItemDataToContents(), StorageData);
			}
			else
			{
				InventoryComponent->AddItem(InvenData);

				SaverInterface->RemoveInvenWithStorage(InventoryComponent->PermuteItemDataToContents(), StorageData);

			}


			RefreshInventory();
		}
	}
}
////////////////////////////////////////////////////////////

//클릭 아이템의 액션메뉴를 생성함//

/// ///////////////////////////////

//모든무기의 아이템을 생성//
void UCPInventorySlotWidget::OnWeaponClicked()
{
	WidgetItemType = EItemType::Weapon;

	RefreshInventory();
}

//모든헬멧의 아이템을 생성//
void UCPInventorySlotWidget::OnHelmetClicked()
{
	WidgetItemType = EItemType::Helmet;

	RefreshInventory();
}

//모든갑옷의 아이템을 생성//
void UCPInventorySlotWidget::OnArmorClicked()
{
	WidgetItemType = EItemType::Armor;

	RefreshInventory();
}

//모든바지의 아이템을 생성//
void UCPInventorySlotWidget::OnPantClicked()
{
	WidgetItemType = EItemType::Pant;

	RefreshInventory();
}

//모든부츠의 아이템을 생성//
void UCPInventorySlotWidget::OnBootsClicked()
{
	WidgetItemType = EItemType::Boots;

	RefreshInventory();
}

//모든소비의 아이템을 생성//
void UCPInventorySlotWidget::OnExpendableClicked()
{
	WidgetItemType = EItemType::Expendable;

	RefreshInventory();
}
void UCPInventorySlotWidget::OnSpellClicked()
{
	WidgetItemType = EItemType::Spell;

	RefreshInventory();
}
void UCPInventorySlotWidget::OnArrowClicked()
{
	WidgetItemType = EItemType::Arrow;

	RefreshInventory();
}
void UCPInventorySlotWidget::OnEnhancementClicked()
{
	WidgetItemType = EItemType::Enhancement;

	RefreshInventory();
}
/////////////////////////////
