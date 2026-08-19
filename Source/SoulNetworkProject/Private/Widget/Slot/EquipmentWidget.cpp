// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/EquipmentWidget.h"
#include "Widget/Slot/Equipment/EquipmentType_SlotWidget.h"
#include "SoulNetworkProject/ItemState.h"
#include "Classes/InventoryComponent.h"

#include "SoulNetworkProject/ItemData.h"


#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Delegates/DelegateSignatureImpl.inl"

#define NOTEXIST FText::FromString("-")
#define FIRSTSLOT 0
#define SECONDSLOT 1
#define THIRDSLOT 2

UEquipmentWidget::UEquipmentWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> GradeRowClass_C(TEXT("WidgetBlueprint'/Game/Widget/Item/Equip/EquipInvenGradeSlot_BP.EquipInvenGradeSlot_BP_C'"));
	if (GradeRowClass_C.Succeeded())
	{
		GradeRowClass = GradeRowClass_C.Class;
	}
}

bool UEquipmentWidget::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	//WidgetText->TextDelegate.BindUFunction(this, "UpdateWidgetText");

	if(EquipMenu) EquipMenu->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledMenu");
	if(SelectMenu) SelectMenu->bIsEnabledDelegate.BindUFunction(this, "UpdateEnabledMenu");

	return true;
}

void UEquipmentWidget::NativeConstruct()
{
	Super::NativeConstruct();

	L_WeaponSlotArray.Add(L_Weapon_Fir);
	L_WeaponSlotArray.Add(L_Weapon_Sec);
	L_WeaponSlotArray.Add(L_Weapon_Thi);

	R_WeaponSlotArray.Add(R_Weapon_Fir);
	R_WeaponSlotArray.Add(R_Weapon_Sec);
	R_WeaponSlotArray.Add(R_Weapon_Thi);

	ExpendableSlotArray.Add(Expendable_1);
	ExpendableSlotArray.Add(Expendable_2);
	ExpendableSlotArray.Add(Expendable_3);
	ExpendableSlotArray.Add(Expendable_4);
	ExpendableSlotArray.Add(Expendable_5);
	ExpendableSlotArray.Add(Expendable_6);
	ExpendableSlotArray.Add(Expendable_7);
	ExpendableSlotArray.Add(Expendable_8);

	ArrowSlotArray.Add(FirstArrowSlot);
	ArrowSlotArray.Add(SecondArrowSlot);

	AllSetSlotType();			//���â Ÿ�԰� �ε�������//
	Info.ClearInfo();			//�������ý� �� ������ ���µ��� ������//
	RefreshEquipment();			//���â�� ���ΰ�ħ��//

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UEquipmentWidget::RefreshItemSlot);
		InventoryComponent->OnEquipmentUpdated.AddDynamic(this, &UEquipmentWidget::RefreshEquipment);
	}

	ChangeInventorySlot(EItemType::Weapon);
	AlReadyGradeSlot();
}

//���� Ÿ�����ϱ�//
void UEquipmentWidget::AllSetSlotType()
{
	//���� ����//
	for (int8 i = 0; i < L_WeaponSlotArray.Num(); i++)
	{
		L_WeaponSlotArray[i]->SetSlotType(EItemType::Weapon, i, EEquipHand::Left);
	}
	for (int8 i = 0; i < R_WeaponSlotArray.Num(); i++)
	{
		R_WeaponSlotArray[i]->SetSlotType(EItemType::Weapon, i, EEquipHand::Right);
	}
	////////////

	//��� ����//
	HelmetSlot->SetSlotType(EItemType::Helmet);
	ArmorSlot->SetSlotType(EItemType::Armor);
	PantSlot->SetSlotType(EItemType::Pant);
	BootsSlot->SetSlotType(EItemType::Boots);
	////////////

	//�Һ�ǰ ����//
	for (int8 i = 0; i < ExpendableSlotArray.Num(); i++)
	{
		ExpendableSlotArray[i]->SetSlotType(EItemType::Expendable, i);
	}
	/////////////

	//ArrowSlot->SetSlotType(EItemType::Arrow);
	for (int8 i = 0; i < ArrowSlotArray.Num(); i++)
	{
		ArrowSlotArray[i]->SetSlotType(EItemType::Arrow, i);
	}
}
////////////////////

//���� �޴��� ����//
void UEquipmentWidget::OnSelectMenu(EItemType ItemType)
{
	SlotSwitcher->SetActiveWidget(SelectMenu);
	ChangeInventorySlot(ItemType);
}
////////////////////

//Hoved �̸� ������Ʈ//
void UEquipmentWidget::UpdateWidgetText(UEquipmentType_SlotWidget* InSlot)
{
	FText Text = FText::FromString("-");

	switch (InSlot->GetSlotType())
	{
		case EItemType::All :
		{
			Text = FText::FromString("All");
		}
		break;
		case EItemType::Weapon:
		{
			FString PermuteString = "";
			if (InSlot->GetHandType() == EEquipHand::Left)
			{
				PermuteString = "LeftWeapon_";
			}
			else if (InSlot->GetHandType() == EEquipHand::Right)
			{
				PermuteString = "RightWeapon_";
			}
			PermuteString += FString::FromInt(InSlot->GetSlotIndex());
			
			InSlot->GetHandType() == EEquipHand::None ? Text = NOTEXIST : Text = FText::FromString(PermuteString);
		}
		break;
		case EItemType::Helmet:
		{
			Text = FText::FromString("Helmet");
		}
		break;
		case EItemType::Armor:
		{
			Text = FText::FromString("Armor");
		}
		break;
		case EItemType::Pant:
		{
			Text = FText::FromString("Pant");
		}
		break;
		case EItemType::Boots:
		{
			Text = FText::FromString("Boots");
		}
		break;
		case EItemType::Arrow:
		{
			Text = FText::FromString("Arrow");
		}
		break;
		case EItemType::Expendable:
		{
			Text = FText::FromString("Expendable");
		}
		break;
		default:
		{
			Text = NOTEXIST;
		}
		break;
	}
	
	UpdatedWidget(InSlot);

	Info.SetInfo(InSlot->GetSlotIndex(), InSlot->GetHandType(), InSlot->GetSlotType());
	WidgetText->SetText(Text);
}
///////////////////


//Data�� ����//
void UEquipmentWidget::InOutItemSlot(USlotWidget* ItemSlot)
{
	EItemType ItemType = ItemSlot->SlotData.ItemData.ItemType;
	switch (ItemType)
	{
		//����//
		case EItemType::Weapon:
		{
			//Info�� �°� �ش� ������ ������ �ľ��� �����Ŵ.//
			if (Info.HandType == EEquipHand::Left ? L_WeaponSlotArray[Info.SlotIndex]->SlotData.IsSet() : R_WeaponSlotArray[Info.SlotIndex]->SlotData.IsSet())
			{
				//�ش罽�Կ� �Ȱ��� �������ϰ�� ��������. (�ش� break�� ItemType�� ������ �ش���.)//
				if (*InventoryComponent->GetItem(EItemType::Weapon, Info.SlotIndex, Info.HandType) == ItemSlot->SlotData)
				{
					InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, Info.SlotIndex, Info.HandType);
					break;
				}
				//�ش罽���� �������� �ٸ���� ������ ��� ������.//
				else
				{
					InventoryComponent->RemoveEquipItem(*InventoryComponent->GetItem(EItemType::Weapon, Info.SlotIndex, Info.HandType), Info.SlotIndex, Info.HandType);
				}
			}
			
			//���� ���� �ݺ���.(�ش� break�� for�� ������ �ش���.)//
			for (uint8 i = 0; i < 3; i++)
			{
				if (FInventoryData* Item = InventoryComponent->GetItem(EItemType::Weapon, i, EEquipHand::Left))
				{
					if (*Item == ItemSlot->SlotData)
					{
						InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, i, EEquipHand::Left);
						break;
	
					}
				}
			}
			//������ ���� �ݺ���.(���� ����.)//
			for (uint8 i = 0; i < 3; i++)
			{
				if (FInventoryData* Item = InventoryComponent->GetItem(EItemType::Weapon, i, EEquipHand::Right))
				{
					if (*Item == ItemSlot->SlotData)
					{
						InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, i, EEquipHand::Right);
	
						break;
					}
				}
			}
	
			InventoryComponent->AddEquipItem(*InventoryComponent->GetIndexDataFromArray(ItemSlot->SlotData), Info.SlotIndex, Info.HandType);
		}
		break;
		///////
	
		//���//
		case EItemType::Helmet:
		{
			if (*InventoryComponent->GetItem(EItemType::Helmet) == ItemSlot->SlotData)
			{
				InventoryComponent->RemoveEquipItem(ItemSlot->SlotData);
			}
			else
			{
				InventoryComponent->RemoveEquipItem(HelmetSlot->SlotData);
				InventoryComponent->AddEquipItem(*InventoryComponent->GetIndexDataFromArray(ItemSlot->SlotData));
			}
		}
		break;
		////////
	
		//����//
		case EItemType::Armor:
		{
			if (*InventoryComponent->GetItem(EItemType::Armor) == ItemSlot->SlotData)
			{
				InventoryComponent->RemoveEquipItem(ItemSlot->SlotData);
			}
			else
			{
				InventoryComponent->RemoveEquipItem(ArmorSlot->SlotData);
				InventoryComponent->AddEquipItem(*InventoryComponent->GetIndexDataFromArray(ItemSlot->SlotData));
			}
		}
		break;
		/////////
	
		//����//
		case EItemType::Pant:
		{
			if (*InventoryComponent->GetItem(EItemType::Pant) == ItemSlot->SlotData)
			{
				InventoryComponent->RemoveEquipItem(ItemSlot->SlotData);
			}
			else
			{
				InventoryComponent->RemoveEquipItem(PantSlot->SlotData);
				InventoryComponent->AddEquipItem(*InventoryComponent->GetIndexDataFromArray(ItemSlot->SlotData));
			}
		}
		break;
		////////
	
		//�Ź�//
		case EItemType::Boots:
		{
			if (*InventoryComponent->GetItem(EItemType::Boots) == ItemSlot->SlotData)
			{
				InventoryComponent->RemoveEquipItem(ItemSlot->SlotData);
			}
			else
			{
				InventoryComponent->RemoveEquipItem(BootsSlot->SlotData);
				InventoryComponent->AddEquipItem(*InventoryComponent->GetIndexDataFromArray(ItemSlot->SlotData));
			}
		}
		break;
		////////

		//ȭ��//
		case EItemType::Arrow:
		{
			// �ش� ���Կ� �������� ���� ���. //
			if (ArrowSlotArray[Info.SlotIndex]->SlotData.IsSet())
			{
				// �ش� ������ ���� �������� ��� ���� �� break ��(break�� switch���� �ش���). //
				if (*InventoryComponent->GetItem(EItemType::Arrow, Info.SlotIndex) == ItemSlot->SlotData)
				{
					InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, Info.SlotIndex);
					break;
				}
				// �ش� ������ �ٸ� �������� ��� ������. //
				else
				{
					InventoryComponent->RemoveEquipItem(*InventoryComponent->GetItem(EItemType::Arrow, Info.SlotIndex));
				}
			}

			// �ش� �������� �ٸ� ���Կ� ���� ���ִ��� ���� �� ���� ��. //
			for (uint8 i = 0; i < ArrowSlotArray.Num(); i++)
			{
				if (ArrowSlotArray[i]->SlotData == ItemSlot->SlotData)
				{
					InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, i);
					break;
				}
			}

			// �ش� �������� ���� ��. //
			InventoryComponent->AddEquipItem(ItemSlot->SlotData, Info.SlotIndex);
		}
		break;
		////////
	
		//�Ҹ�ǰ//
		case EItemType::Expendable:
		{
			//operator ==�� ������ IsSet���� �̿��� �������� �������� �������� �������� �����ϰ� ����//
			if (ExpendableSlotArray[Info.SlotIndex]->SlotData.IsSet())
			{
				//������ �������� ���� ���Ծ����۰� ���� ��� ��ü���� �ش� break�� ItemType ����ġ���� �ش���.//
				if (*InventoryComponent->GetItem(EItemType::Expendable, Info.SlotIndex) == ItemSlot->SlotData)
				{
					InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, Info.SlotIndex);
					InventoryComponent->RefreshExpendableIndex();
					break;
				}
				//�ٸ� �������� ��� ������ ��.//
				else
				{
					InventoryComponent->RemoveEquipItem(*InventoryComponent->GetItem(EItemType::Expendable, Info.SlotIndex));
				}
			}
			
			//for������ Ž���� �ش�����۰� ���� �������� ������ ������ ������. (�ش� break�� Ž���ϴ� for���� �ش���.)//
			for (uint8 i = 0; i < ExpendableSlotArray.Num(); i++)
			{
				if (ExpendableSlotArray[i]->SlotData == ItemSlot->SlotData)
				{
					InventoryComponent->RemoveEquipItem(ItemSlot->SlotData, i);
					break;
				}
			}
			InventoryComponent->AddEquipItem(ItemSlot->SlotData, Info.SlotIndex);
			InventoryComponent->RefreshExpendableIndex();
		}
		break;
		//////////
	
		default:
			break;
	}
	ChangeInventorySlot(ItemType);
}

void UEquipmentWidget::BackEquipMenu()
{
	SlotSwitcher->SetActiveWidget(EquipMenu);
	PlayAnimation(EquipBlink);
}

bool UEquipmentWidget::IsActiveEquipMenu()
{
	if(SelectMenu == SlotSwitcher->GetActiveWidget()) return false;

	return true;
}

//Data�� ����//
void UEquipmentWidget::RefreshEquipment()
{
	//Left weapon//
	for (int8 i = 0; i < L_WeaponSlotArray.Num(); i++)
	{
		if (FInventoryData* Data = InventoryComponent->L_WeaponEquip.Find(i))
		{
			if(Data->IsSet()) L_WeaponSlotArray[i]->SetItemSlot(*Data);
		}
		else
		{
			L_WeaponSlotArray[i]->RemoveSlot();
		}
	}
	
	
	//Right Weapon//
	for (int8 i = 0; i < R_WeaponSlotArray.Num(); i++)
	{
		if (FInventoryData* Data = InventoryComponent->R_WeaponEquip.Find(i))
		{
			if (Data->IsSet()) R_WeaponSlotArray[i]->SetItemSlot(*Data);
		}
		else
		{
			R_WeaponSlotArray[i]->RemoveSlot();
		}
	}
	
	//Equip Armor//
	if (InventoryComponent->HelmetEquip.IsSet()) HelmetSlot->SetItemSlot(InventoryComponent->HelmetEquip);
	else HelmetSlot->RemoveSlot();
	if (InventoryComponent->ArmorEquip.IsSet()) ArmorSlot->SetItemSlot(InventoryComponent->ArmorEquip);
	else ArmorSlot->RemoveSlot();
	if (InventoryComponent->PantEquip.IsSet()) PantSlot->SetItemSlot(InventoryComponent->PantEquip);
	else PantSlot->RemoveSlot();
	if (InventoryComponent->BootsEquip.IsSet()) BootsSlot->SetItemSlot(InventoryComponent->BootsEquip);
	else BootsSlot->RemoveSlot();
	///////////////
	
	//Expendable//
	for (int8 i = 0; i < ExpendableSlotArray.Num(); i++)
	{
		if (InventoryComponent->GetItem(EItemType::Expendable, i))
		{
			ExpendableSlotArray[i]->SetItemSlot(*InventoryComponent->GetItem(EItemType::Expendable, i));
		}
		else
		{
			ExpendableSlotArray[i]->RemoveSlot();
		}
	}

	for (int8 i = 0; i < ArrowSlotArray.Num(); i++)
	{
		if (InventoryComponent->GetItem(EItemType::Arrow, i))
		{
			ArrowSlotArray[i]->SetItemSlot(*InventoryComponent->GetItem(EItemType::Arrow, i));
		}
		else
		{
			ArrowSlotArray[i]->RemoveSlot();
		}
	}

	//if (InventoryComponent->ArrowEquip.IsSet()) ArrowSlot->SetItemSlot(InventoryComponent->ArrowEquip);
	//else ArrowSlot->RemoveSlot();

	//RefreshItemSlot();
}

void UEquipmentWidget::RefreshItemSlot()
{
	if(Info.ItemType == EItemType::All) return;

	ChangeInventorySlot(Info.ItemType);
}
