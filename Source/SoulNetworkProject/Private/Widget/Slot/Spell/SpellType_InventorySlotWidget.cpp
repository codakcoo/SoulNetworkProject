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


	AllSetSlotType();					//���â Ÿ�԰� �ε�������//
	CurrentSlotIndex.Reset();			//�������ý� �� ������ ���µ��� ������//
	//RefreshEquipment();				//���â�� ���ΰ�ħ��//
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

//���� Ÿ�����ϱ�//
void USpellType_InventorySlotWidget::AllSetSlotType()
{
	//���� �ε���//
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

	
	//operator ==�� ������ IsSet���� �̿��� �������� �������� �������� �������� �����ϰ� ����//
	if (SpellSlotArray[CurrentSlotIndex.GetValue()]->SlotData.IsSet())
	{
		//������ �������� ���� ���Ծ����۰� ���� ��� ��ü���� �ش� break�� ItemType ����ġ���� �ش���.//
		if (SpellSlotArray[CurrentSlotIndex.GetValue()]->SlotData == ItemSlot->SlotData)
		{
			InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, CurrentSlotIndex.GetValue());
			InventoryComponent->RefreshSpellIndex();
		}
		//�ٸ� �������� ��� ������ ��.//
		else
		{
			//�ش� ������ ������ equip�� ������Ŵ.//
			InventoryComponent->RemoveEquipItem(*InventoryComponent->GetItem(EItemType::Spell, CurrentSlotIndex.GetValue()), CurrentSlotIndex.GetValue());
			for (uint8 i = 0; i < SpellSlotArray.Num(); i++)
			{
				//�����ϰ��� �ϴ� �������� �ߺ��Ǵ� ���� �ִ��� �˻� �� �ִ� ��� �ߺ��Ǵ� ���� equip ������Ŵ.//
				if (SpellSlotArray[i]->SlotData == ItemSlot->SlotData)
				{
					InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, i);
					InventoryComponent->RefreshSpellIndex();
					break;
				}
			}
			//����.//
			InventoryComponent->AddEquipItem(ItemSlot->SlotData, CurrentSlotIndex.GetValue());
			InventoryComponent->RefreshSpellIndex();
		}
	}
	else
	{
		//for������ Ž���� �ش�����۰� ���� �������� ������ ������ ������. (�ش� break�� Ž���ϴ� for���� �ش���.)//
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

//Data�� ����//
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

	if (SelectType == ESpellType::None)	//��� �������� �߰�.//
	{
		UGradeSlotWidget* CreateCurrentSlot = CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
		//�켱�������� ���ʴ�� Ž���Ͽ� �ش�´� �������� ���������� �߰�����.//
		for (int8 i = 0; i < EnumObject->GetMaxEnumValue(); i++)
		{
			for (auto& Item : CategoryItems)
			{
				//������ ������ ����� Ÿ�԰� �´� Data�� �߰���Ŵ//
				if (GetItemEnumValue<FInventoryData>(Item) == i)
				{
					//GradeSlot�� �� �̻� �߰��� �� ���ٸ� ���� ��������.//
					if (!CreateCurrentSlot->CanStackData())
					{
						RowIndex++;
						CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
					}

					//������ �Ϸ�Ǿ��� ���.//
					if (CreateCurrentSlot)
					{
						//���� ������ GradeSlot�� ��� GradeBox�� �ڼ����� �߰���Ŵ.//
						if (CreateCurrentSlot->SlotStackCount() == 0)
						{
							GradeBox->AddChild(CreateCurrentSlot);
						}

						//Index�� GradeSlot�� Index�� �����۰� �������� Index�� ��������.//
						CreateCurrentSlot->SetupRowIndex(RowIndex);
						CreateCurrentSlot->UpdatedItemSlot(SlotIndex, Item);
						SlotIndex++;

					}
				}
			}
		}
	}
	else //������ ������ �����۸� �߰�.//
	{
		UGradeSlotWidget* CreateCurrentSlot = CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
		for (auto& Item : CategoryItems)
		{
			//������ ������ ����� Ÿ�԰� �´� Data�� �߰���Ŵ//
			if (GetItemEnumValue<FInventoryData>(Item) == (uint8)SelectType)
			{
				//GradeSlot�� �� �̻� �߰��� �� ���ٸ� ���� ��������.//
				if (!CreateCurrentSlot->CanStackData())
				{
					RowIndex++;
					CreateCurrentSlot = CreateWidget<UGradeSlotWidget>(this, GradeRowClass);
				}

				//������ �Ϸ�Ǿ��� ���.//
				if (CreateCurrentSlot)
				{
					//���� ������ GradeSlot�� ��� GradeBox�� �ڼ����� �߰���Ŵ.//
					if (CreateCurrentSlot->SlotStackCount() == 0)
					{
						GradeBox->AddChild(CreateCurrentSlot);
					}

					//Index�� GradeSlot�� Index�� �����۰� �������� Index�� ��������.//
					CreateCurrentSlot->SetupRowIndex(RowIndex);
					CreateCurrentSlot->UpdatedItemSlot(SlotIndex, Item);
					SlotIndex++;

				}
			}
		}
	}
}
