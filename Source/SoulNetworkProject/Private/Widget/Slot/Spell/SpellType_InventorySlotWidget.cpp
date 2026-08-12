// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Spell/SpellType_InventorySlotWidget.h"
#include "Widget/Slot/Spell/SpellType_SlotWidget.h"
#include "Widget/Slot/GradeSlotWidget.h"

#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"


USpellType_InventorySlotWidget::USpellType_InventorySlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> GradeRowClass_C(TEXT("WidgetBlueprint'/Game/Widget/Item/Spell/SpellGradeSlot_BP.SpellGradeSlot_BP_C'"));
	if (GradeRowClass_C.Succeeded())
	{
		GradeRowClass = GradeRowClass_C.Class;
	}
}

bool USpellType_InventorySlotWidget::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if(WidgetText) WidgetText->TextDelegate.BindUFunction(this, "UpdateWidgetText");
	if (AllSpellButton && AttackSorceryButton && AssistSorceryButton && AttackIncantationButton && AssistIncantationButton)
	{
		AllSpellButton->OnClicked.AddDynamic(this, &USpellType_InventorySlotWidget::OnAllClicked);
		AllSpellButton->bIsEnabledDelegate.BindUFunction(this, "CheckAll");

		AttackSorceryButton->OnClicked.AddDynamic(this, &USpellType_InventorySlotWidget::OnAAttackSorceryClicked);
		AttackSorceryButton->bIsEnabledDelegate.BindUFunction(this, "CheckAttackSorcery");

		AssistSorceryButton->OnClicked.AddDynamic(this, &USpellType_InventorySlotWidget::OnAssistSorceryClicked);
		AssistSorceryButton->bIsEnabledDelegate.BindUFunction(this, "CheckAssistSorcery");

		AttackIncantationButton->OnClicked.AddDynamic(this, &USpellType_InventorySlotWidget::OnAttackIncantationClicked);
		AttackIncantationButton->bIsEnabledDelegate.BindUFunction(this, "CheckAttackIncantation");

		AssistIncantationButton->OnClicked.AddDynamic(this, &USpellType_InventorySlotWidget::OnAssistIncantationClicked);
		AssistIncantationButton->bIsEnabledDelegate.BindUFunction(this, "CheckAssistIncantation");
	}


	return true;
}

void USpellType_InventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SpellSlotArray.Add(SpellSlot_Fir);
	SpellSlotArray.Add(SpellSlot_Sec);
	SpellSlotArray.Add(SpellSlot_Thi);
	SpellSlotArray.Add(SpellSlot_For);


	AllSetSlotType();					//장비창 타입과 인덱스설정//
	CurrentSlotIndex.Reset();			//슬폿선택시 그 슬롯의 상태들을 가져옴//
	//RefreshEquipment();				//장비창을 새로고침함//
	BackEquipSlot();
	RefreshSpellMenu();
	RefreshItemSlot();
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryUpdated.AddDynamic(this, &USpellType_InventorySlotWidget::RefreshItemSlot);
		InventoryComponent->OnEquipmentUpdated.AddDynamic(this, &USpellType_InventorySlotWidget::RefreshSpellMenu);
	}
}

void USpellType_InventorySlotWidget::CallFirstMenu()
{
	BackEquipSlot();
	SelectType = ESpellType::None;
	RefreshItemSlot();

	//PlayAnimation(Blink);
}

//슬롯 타입정하기//
void USpellType_InventorySlotWidget::AllSetSlotType()
{
	//스펠 인덱스//
	for (int8 i = 0; i < SpellSlotArray.Num(); i++)
	{
		SpellSlotArray[i]->SetSlotIndex(i);
	}
	//////////////
}
void USpellType_InventorySlotWidget::OnSelectMenu()
{
	//if(!CurrentSlotIndex.IsSet()) return;

	SpellSlotArray[CurrentSlotIndex.GetValue()]->SetSelect(true);
	ActiveSelectSlot();
}
void USpellType_InventorySlotWidget::UpdateEquipWidgetText(USpellType_SlotWidget* InSlot)
{
	//if(InSlot->SlotData.ItemData.ItemType != EItemType::Spell) return;
	
	FText Text = FText::FromString("-");
	Text = FText::FromName(InSlot->SlotData.ItemData.ItemName);



	//UpdatedWidget(InSlot);

	CurrentSlotIndex = InSlot->GetSlotIndex();
	WidgetText->SetText(Text);
}

void USpellType_InventorySlotWidget::UpdateInvenWidgetText(USpellType_InvenSlotWidget* InSlot)
{
}

void USpellType_InventorySlotWidget::InOutItemSlot(USlotWidget* ItemSlot)
{
	EItemType ItemType = ItemSlot->SlotData.ItemData.ItemType;
	if(!ItemSlot->SlotData.IsSet() || ItemType != EItemType::Spell || !CurrentSlotIndex.IsSet() || MenuType != EMenuType_Spell::Select) return;

	
	//operator ==의 오류로 IsSet으로 이용해 아이템이 있을때와 없을때를 구분지어 통콰하게 만듬//
	if (SpellSlotArray[CurrentSlotIndex.GetValue()]->SlotData.IsSet())
	{
		//슬롯의 아이템이 선택 슬롯아이템과 같을 경우 해체만함 해당 break는 ItemType 스위치문에 해당함.//
		if (SpellSlotArray[CurrentSlotIndex.GetValue()]->SlotData == ItemSlot->SlotData)
		{
			InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, CurrentSlotIndex.GetValue());
			InventoryComponent->RefreshSpellIndex();
		}
		//다른 아이템일 경우 해제만 함.//
		else
		{
			//해당 아이템 슬롯의 equip을 해제시킴.//
			InventoryComponent->RemoveEquipItem(*InventoryComponent->GetItem(EItemType::Spell, CurrentSlotIndex.GetValue()), CurrentSlotIndex.GetValue());
			for (uint8 i = 0; i < SpellSlotArray.Num(); i++)
			{
				//변경하고자 하는 아이템이 중복되는 곳이 있는지 검사 후 있는 경우 중복되는 곳을 equip 해제시킴.//
				if (SpellSlotArray[i]->SlotData == ItemSlot->SlotData)
				{
					InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, i);
					InventoryComponent->RefreshSpellIndex();
					break;
				}
			}
			//장착.//
			InventoryComponent->AddEquipItem(ItemSlot->SlotData, CurrentSlotIndex.GetValue());
			InventoryComponent->RefreshSpellIndex();
		}
	}
	else
	{
		//for문으로 탐색후 해당아이템과 같은 아이템이 있으면 해제후 장착함. (해당 break는 탐색하는 for문에 해당함.)//
		for (uint8 i = 0; i < SpellSlotArray.Num(); i++)
		{
			if (SpellSlotArray[i]->SlotData == ItemSlot->SlotData)
			{
				InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, i);
				InventoryComponent->RefreshSpellIndex();
				break;
			}
		}
		InventoryComponent->AddEquipItem(ItemSlot->SlotData, CurrentSlotIndex.GetValue());
		InventoryComponent->RefreshSpellIndex();
	}

	BackEquipSlot();
}

////////////////////

void USpellType_InventorySlotWidget::BackEquipSlot()
{
	if(CurrentSlotIndex.IsSet()) SpellSlotArray[CurrentSlotIndex.GetValue()]->SetSelect(false);


	EquipSpellSlotsBox->SetIsEnabled(true);

	SelectMenuBox->SetIsEnabled(false);
	InvenMenuBox->SetIsEnabled(false);

	CurrentSlotIndex.Reset();
	MenuType = EMenuType_Spell::Equip;
}

void USpellType_InventorySlotWidget::ActiveSelectSlot()
{
	EquipSpellSlotsBox->SetIsEnabled(false);

	SelectMenuBox->SetIsEnabled(true);
	InvenMenuBox->SetIsEnabled(true);
	MenuType = EMenuType_Spell::Select;
}

void USpellType_InventorySlotWidget::OnAllClicked()
{
	if(MenuType != EMenuType_Spell::Select) return;

	SelectType = ESpellType::None;
	RefreshItemSlot();
}

void USpellType_InventorySlotWidget::OnAAttackSorceryClicked()
{
	if (MenuType != EMenuType_Spell::Select) return;

	SelectType = ESpellType::AttackSorcery;
	RefreshItemSlot();
}

void USpellType_InventorySlotWidget::OnAssistSorceryClicked()
{
	if (MenuType != EMenuType_Spell::Select) return;

	SelectType = ESpellType::AssistSorcery;
	RefreshItemSlot();
}

void USpellType_InventorySlotWidget::OnAttackIncantationClicked()
{
	if (MenuType != EMenuType_Spell::Select) return;

	SelectType = ESpellType::AttackIncantation;
	RefreshItemSlot();
}

void USpellType_InventorySlotWidget::OnAssistIncantationClicked()
{
	if (MenuType != EMenuType_Spell::Select) return;

	SelectType = ESpellType::AssistIncantation;
	RefreshItemSlot();
}

void USpellType_InventorySlotWidget::SwitchingMenu(bool bForward)
{
	if (MenuType == EMenuType_Spell::Equip) return;

	if (bForward)
	{
		uint8 Index = static_cast<uint8>(SelectType) + 1;
		SelectType = static_cast<ESpellType>(Index);
		if ((uint8)SelectType > (uint8)ESpellType::AssistIncantation) SelectType = ESpellType::None;
	}
	else
	{
		uint8 Index = static_cast<uint8>(SelectType) + 1;
		SelectType = static_cast<ESpellType>(Index);
		if ((uint8)SelectType < (uint8)ESpellType::None) SelectType = ESpellType::AssistIncantation;
	}

	RefreshItemSlot();
}

bool USpellType_InventorySlotWidget::IsActiveEquipMenu()
{
	//if (SelectMenu == SlotSwitcher->GetActiveWidget()) return false;
	//
	return true;
}

//Data로 변형//
void USpellType_InventorySlotWidget::RefreshSpellMenu()
{
	for (int8 i = 0; i < SpellSlotArray.Num(); i++)
	{
		if (FInventoryData* Data = InventoryComponent->SpellEquip.Find(i))
		{
			if(Data->IsSet()) SpellSlotArray[i]->SetItemSlot(*Data);
		}
		else
		{
			SpellSlotArray[i]->RemoveSlot();
		}
	}

	RefreshItemSlot();
}

void USpellType_InventorySlotWidget::RefreshItemSlot()
{
	PlayAnimation(InvenBlink);
	GradeBox->ClearChildren();
	TArray<FInventoryData> CategoryItems;

	for (auto& Item : InventoryComponent->Items)
	{
		if ((uint8)Item.ItemData.ItemType == (uint8)EItemType::Spell)
		{
			CategoryItems.Add(Item);
		}
	}
	if (CategoryItems.Num() == 0) return;

	uint8 RowIndex = 0;
	uint8 SlotIndex = 0;

	const UEnum* EnumObject = GetEnumObject(EItemType::Spell);
	if (!EnumObject) return;

	if (SelectType == ESpellType::None)	//모든 스펠종류 추가.//
	{
		UGradeSlotWidget* CreateCurrentSlot = CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
		//우선순위부터 차례대로 탐색하여 해당맞는 아이템을 순차적으로 추가해줌.//
		for (int8 i = 0; i < EnumObject->GetMaxEnumValue(); i++)
		{
			for (auto& Item : CategoryItems)
			{
				//만약의 오류를 대비해 타입과 맞는 Data만 추가시킴//
				if (GetItemEnumValue<FInventoryData>(Item) == i)
				{
					//GradeSlot이 더 이상 추가할 수 없다면 새로 생성해줌.//
					if (!CreateCurrentSlot->CanStackData())
					{
						RowIndex++;
						CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
					}

					//생성이 완료되었을 경우.//
					if (CreateCurrentSlot)
					{
						//새로 생성된 GradeSlot일 경우 GradeBox에 자손으로 추가시킴.//
						if (CreateCurrentSlot->SlotStackCount() == 0)
						{
							GradeBox->AddChild(CreateCurrentSlot);
						}

						//Index로 GradeSlot의 Index와 아이템과 아이템의 Index를 지정해줌.//
						CreateCurrentSlot->SetupRowIndex(RowIndex);
						CreateCurrentSlot->UpdatedItemSlot(SlotIndex, Item);
						SlotIndex++;

					}
				}
			}
		}
	}
	else //선택한 슬롯의 아이템만 추가.//
	{
		UGradeSlotWidget* CreateCurrentSlot = CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
		for (auto& Item : CategoryItems)
		{
			//만약의 오류를 대비해 타입과 맞는 Data만 추가시킴//
			if (GetItemEnumValue<FInventoryData>(Item) == (uint8)SelectType)
			{
				//GradeSlot이 더 이상 추가할 수 없다면 새로 생성해줌.//
				if (!CreateCurrentSlot->CanStackData())
				{
					RowIndex++;
					CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
				}

				//생성이 완료되었을 경우.//
				if (CreateCurrentSlot)
				{
					//새로 생성된 GradeSlot일 경우 GradeBox에 자손으로 추가시킴.//
					if (CreateCurrentSlot->SlotStackCount() == 0)
					{
						GradeBox->AddChild(CreateCurrentSlot);
					}

					//Index로 GradeSlot의 Index와 아이템과 아이템의 Index를 지정해줌.//
					CreateCurrentSlot->SetupRowIndex(RowIndex);
					CreateCurrentSlot->UpdatedItemSlot(SlotIndex, Item);
					SlotIndex++;

				}
			}
		}
	}
}
