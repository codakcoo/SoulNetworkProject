// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/ClothComponent.h"
#include "Classes/PlayerCharacter.h"
#include "SoulNetworkProject/ItemState.h"
#include "SoulNetworkLog.h"


#define CHEST_TAG TEXT("BODY_CHEST")
#define ARM_TAG TEXT("BODY_ARMS")
#define HAND_TAG TEXT("BODY_HANDS")
#define LEG_TAG TEXT("BODY_LEDGS")
#define FEET_TAG TEXT("BODY_FEETS")

#define HELMET_TAG TEXT("HELMET")
#define ARMOR_TAG TEXT("ARMOR")
#define PANT_TAG TEXT("PANT")
#define BOOTS_TAG TEXT("BOOTS")

// Sets default values for this component's properties
UClothComponent::UClothComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UClothComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Owner = Cast<APlayerCharacter>(GetOwner());
}

void UClothComponent::AddBodySlot(FItemData Item, FClothData Cloth)
{
	USkeletalMeshComponent* BodyComponent = nullptr;
	USkeletalMeshComponent* ClothComponent = nullptr;

	if (!Item.IsSet()) return;

	if (Cloth.BodyTypes.Num() != 0)
	{
		for (EBodyType& Body : Cloth.BodyTypes)
		{
			BodyComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), FName(*EnumToString(Body)))[0]);
			BodyComponent->SetHiddenInGame(true);
		}
	}

	switch (Item.ItemType)
	{
	case EItemType::Weapon:
		return;

	case EItemType::Helmet:
	{
		//ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), HELMET_TAG)[0]);
		//if (!ClothComponent) return;
		//ClothComponent->SetSkeletalMesh(Item.Mesh);

		Owner->GetHelmetMesh()->SetSkeletalMesh(Item.Mesh);
	}
	break;

	case EItemType::Armor:
	{
		//ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), ARMOR_TAG)[0]);
		//if (!ClothComponent) return;
		//ClothComponent->SetSkeletalMesh(Item.Mesh);

		Owner->GetArmorMesh()->SetSkeletalMesh(Item.Mesh);
	}
	break;

	case EItemType::Pant:
	{
		//ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), PANT_TAG)[0]);
		//if (!ClothComponent) return;
		//ClothComponent->SetSkeletalMesh(Item.Mesh);

		Owner->GetPantMesh()->SetSkeletalMesh(Item.Mesh);
	}
	break;

	case EItemType::Boots:
	{
		//ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), BOOTS_TAG)[0]);
		//if (!ClothComponent) return;
		//ClothComponent->SetSkeletalMesh(Item.Mesh);

		Owner->GetBootsMesh()->SetSkeletalMesh(Item.Mesh);
	}
	break;

	default:
		break;
	}
}


void UClothComponent::Server_AddBodySlot_Implementation(FItemData Item, FClothData Cloth)
{
	Multicast_AddBodySlot(Item, Cloth);
}

void UClothComponent::Multicast_AddBodySlot_Implementation(FItemData Item, FClothData Cloth)
{
	//USkeletalMeshComponent* BodyComponent = nullptr;
	//USkeletalMeshComponent* ClothComponent = nullptr;
	//
	//if (!Item.IsSet()) return;
	//
	//if (Cloth.BodyTypes.Num() != 0)
	//{
	//	for (EBodyType& Body : Cloth.BodyTypes)
	//	{
	//		BodyComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), FName(*EnumToString(Body)))[0]);
	//		BodyComponent->SetHiddenInGame(true);
	//	}
	//}
	//
	//switch (Item.ItemType)
	//{
	//	case EItemType::Weapon:
	//		return;
	//
	//	case EItemType::Helmet:
	//	{
	//		
	//		ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), HELMET_TAG)[0]);
	//		ClothComponent->SetSkeletalMesh(Item.Mesh);
	//	}
	//	break;
	//
	//	case EItemType::Armor:
	//	{
	//		ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), ARMOR_TAG)[0]);
	//		ClothComponent->SetSkeletalMesh(Item.Mesh);
	//	}
	//	break;
	//
	//	case EItemType::Pant:
	//	{
	//		ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), PANT_TAG)[0]);
	//		ClothComponent->SetSkeletalMesh(Item.Mesh);
	//	}
	//	break;
	//
	//	case EItemType::Boots:
	//	{
	//		ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), BOOTS_TAG)[0]);
	//		ClothComponent->SetSkeletalMesh(Item.Mesh);
	//	}
	//	break;
	//
	//	default:
	//		break;
	//}
	//BodySlot(Item, Cloth);

	if (!Owner) return;

	USkeletalMeshComponent* BodyComponent = nullptr;
	USkeletalMeshComponent* ClothComponent = nullptr;

	if (!Item.IsSet()) return;

	if (Cloth.BodyTypes.Num() != 0)
	{
		for (EBodyType& Body : Cloth.BodyTypes)
		{
			BodyComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), FName(*EnumToString(Body)))[0]);
			BodyComponent->SetHiddenInGame(true);
		}
	}

	switch (Item.ItemType)
	{
	case EItemType::Helmet:
	{
		Owner->GetHelmetMesh()->SetSkeletalMesh(Item.Mesh);
	}
	break;

	case EItemType::Armor:
	{
		Owner->GetArmorMesh()->SetSkeletalMesh(Item.Mesh);
	}
	break;

	case EItemType::Pant:
	{
		Owner->GetPantMesh()->SetSkeletalMesh(Item.Mesh);
	}
	break;

	case EItemType::Boots:
	{
		Owner->GetBootsMesh()->SetSkeletalMesh(Item.Mesh);
	}
	break;
	}
}

void UClothComponent::RemoveBodySlot(FItemData Item, FClothData Cloth)
{
	if (!Item.IsSet()) return;

	USkeletalMeshComponent* BodyComponent = nullptr;
	USkeletalMeshComponent* ClothComponent = nullptr;

	switch (Item.ItemType)
	{
	case EItemType::Weapon:
		return;

	case EItemType::Helmet:
	{
		ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), HELMET_TAG)[0]);
		ClothComponent->SetSkeletalMesh(nullptr);
	}
	break;

	case EItemType::Armor:
	{
		ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), ARMOR_TAG)[0]);
		ClothComponent->SetSkeletalMesh(nullptr);
	}
	break;

	case EItemType::Pant:
	{
		ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), PANT_TAG)[0]);
		ClothComponent->SetSkeletalMesh(nullptr);
	}
	break;

	case EItemType::Boots:
	{
		ClothComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), BOOTS_TAG)[0]);
		ClothComponent->SetSkeletalMesh(nullptr);
	}
	break;

	default:
		return;
	}

	if (Cloth.BodyTypes.Num() != 0)
	{
		for (const EBodyType& Body : Cloth.BodyTypes)
		{
			BodyComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), FName(*EnumToString(Body)))[0]);
			BodyComponent->SetHiddenInGame(false);
			UE_LOG(LogSoulGame, Warning, TEXT("Cloth : %s"), *EnumToString(Body));
		}
	}
}

void UClothComponent::RefreshClothMesh()
{
	if (Owner)
	{
		if (Owner->Get_E_Helmet().IsSet()) Server_AddBodySlot(Owner->Get_E_Helmet().ItemData, Owner->Get_E_Helmet().ClothData);
		if (Owner->Get_E_Armor().IsSet()) Server_AddBodySlot(Owner->Get_E_Armor().ItemData, Owner->Get_E_Armor().ClothData);
		if (Owner->Get_E_Pant().IsSet()) Server_AddBodySlot(Owner->Get_E_Pant().ItemData, Owner->Get_E_Pant().ClothData);
		if (Owner->Get_E_Boots().IsSet()) Server_AddBodySlot(Owner->Get_E_Boots().ItemData, Owner->Get_E_Boots().ClothData);
	}
}


void UClothComponent::Server_RemoveBodySlot_Implementation(FItemData Item, FClothData Cloth)
{
	Multicast_RemoveBodySlot(Item, Cloth);
}

void UClothComponent::Multicast_RemoveBodySlot_Implementation(FItemData Item, FClothData Cloth)
{
	if (!Owner) return;

	USkeletalMeshComponent* BodyComponent = nullptr;
	USkeletalMeshComponent* ClothComponent = nullptr;

	if (!Item.IsSet()) return;

	if (Cloth.BodyTypes.Num() != 0)
	{
		for (EBodyType& Body : Cloth.BodyTypes)
		{
			BodyComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), FName(*EnumToString(Body)))[0]);
			BodyComponent->SetHiddenInGame(false);
		}
	}

	switch (Item.ItemType)
	{
	case EItemType::Helmet:
	{
		Owner->GetHelmetMesh()->SetSkeletalMesh(nullptr);
	}
	break;

	case EItemType::Armor:
	{
		Owner->GetArmorMesh()->SetSkeletalMesh(nullptr);
	}
	break;

	case EItemType::Pant:
	{
		Owner->GetPantMesh()->SetSkeletalMesh(nullptr);
	}
	break;

	case EItemType::Boots:
	{
		Owner->GetBootsMesh()->SetSkeletalMesh(nullptr);
	}
	break;
	}
}

void UClothComponent::BodySlot(FItemData ItemData, FClothData ClothData, bool bActive)
{
	if(!Owner) return;

	USkeletalMeshComponent* BodyComponent = nullptr;
	USkeletalMeshComponent* ClothComponent = nullptr;

	if (!ItemData.IsSet()) return;

	if (ClothData.BodyTypes.Num() != 0)
	{
		for (EBodyType& Body : ClothData.BodyTypes)
		{
			BodyComponent = Cast<USkeletalMeshComponent>(Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), FName(*EnumToString(Body)))[0]);
			BodyComponent->SetHiddenInGame(true);
		}
	}

	switch (ItemData.ItemType)
	{
	case EItemType::Helmet:
	{
		Owner->GetHelmetMesh()->SetSkeletalMesh(ItemData.Mesh);
	}
	break;

	case EItemType::Armor:
	{
		Owner->GetArmorMesh()->SetSkeletalMesh(ItemData.Mesh);
	}
	break;

	case EItemType::Pant:
	{
		Owner->GetPantMesh()->SetSkeletalMesh(ItemData.Mesh);
	}
	break;

	case EItemType::Boots:
	{
		Owner->GetBootsMesh()->SetSkeletalMesh(ItemData.Mesh);
	}
	break;
	}
}

void UClothComponent::AddClothMesh(FItemData ItemData, FClothData ClothData)
{
	if(!GetOwner()) return;


	USkeletalMeshComponent* BodyComponent = nullptr;
	USkeletalMeshComponent* ClothComponent = nullptr;

	if (!ItemData.IsSet()) return;

	if (ClothData.BodyTypes.Num() != 0)
	{
		for (EBodyType& Body : ClothData.BodyTypes)
		{
			BodyComponent = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), FName(*EnumToString(Body)))[0]);
			BodyComponent->SetHiddenInGame(true);
		}
	}

	FName ClothName = NAME_None;
	switch (ItemData.ItemType)
	{
		case EItemType::Helmet:
			ClothName = HELMET_TAG;
			break;
		case EItemType::Armor:
			ClothName = ARMOR_TAG;
			break;
		case EItemType::Pant:
			ClothName = PANT_TAG;
			break;
		case EItemType::Boots:
			ClothName = BOOTS_TAG;
			break;
		default:
			break;
	}
	ClothComponent = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), ClothName)[0]);
	if (ClothComponent)
	{
		ClothComponent->SetSkeletalMesh(ItemData.Mesh);
	}

	//switch (ItemData.ItemType)
	//{
	//	case EItemType::Helmet:
	//	{
	//		Owner->GetHelmetMesh()->SetSkeletalMesh(Item.Mesh);
	//	}
	//	break;
	//
	//	case EItemType::Armor:
	//	{
	//		Owner->GetArmorMesh()->SetSkeletalMesh(Item.Mesh);
	//	}
	//	break;
	//
	//	case EItemType::Pant:
	//	{
	//		Owner->GetPantMesh()->SetSkeletalMesh(Item.Mesh);
	//	}
	//	break;
	//
	//	case EItemType::Boots:
	//	{
	//		Owner->GetBootsMesh()->SetSkeletalMesh(Item.Mesh);
	//	}
	//	break;
	//}
}
