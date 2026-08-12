// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/WorldPickupItem.h"
#include "Classes/PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Interface/DataInterface.h"

#include "NiagaraComponent.h"
#include "SoulNetworkLog.h"


// Sets default values
AWorldPickupItem::AWorldPickupItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetupAttachment(Billboard);
	Collision->SetSphereRadius(45.f);
	Collision->SetIsReplicated(true);


	Niagara = CreateDefaultSubobject<UNiagaraComponent>("Niagara");
	Niagara->SetupAttachment(Collision);
	Niagara->SetIsReplicated(true);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ItemSmoke_Obj(TEXT("NiagaraSystem'/Game/Effect/Niagara/Item/NS_ItemSmoke.NS_ItemSmoke'"));
	if (ItemSmoke_Obj.Succeeded())
	{
		Niagara->SetAsset(ItemSmoke_Obj.Object);
	}

	SetReplicates(true);
	SetReplicatingMovement(true);

	Description = TEXT("Pick up item");
}

// Called when the game starts or when spawned
void AWorldPickupItem::BeginPlay()
{
	Super::BeginPlay();

}

void AWorldPickupItem::Interact(APlayerCharacter* Character)
{
	IDataInterface* Interface = Cast<IDataInterface>(GetWorld()->GetGameInstance());

	for (auto& Content : DataContents)
	{
		Content.HandType = EEquipHand::None;
		if (Interface)
		{
			if (FindRow<FItemData>(Interface->GetItemDataTable(), Content.DataName, "Item Data Context.") == nullptr) return;// nullptr;

			FInventoryData DefaultData;
			DefaultData.ItemData = (*FindRow<FItemData>(Interface->GetItemDataTable(), Content.DataName, "Item Data Context."));

			switch (DefaultData.ItemData.ItemType)
			{
			case EItemType::Weapon:
			{
				if (Content.DataCount > 8) return;// nullptr;
				else if (FWeaponData* WeaponData = FindRow<FWeaponData>(Interface->GetWeaponDataTable(), Content.DataName, "Weapon Data Context."))
				{
					DefaultData.WeaponData = (*WeaponData);
					DefaultData.ItemCount = Content.DataCount;
					DefaultData.HandType = Content.HandType;

					Character->GetInventoryComponent()->AddItem(DefaultData, true);
				}
			}
			break;

			case EItemType::Helmet:
			case EItemType::Armor:
			case EItemType::Pant:
			case EItemType::Boots:
			{
				if (Content.DataCount > 5) return;// nullptr;
				else if (FClothData* ClothData = FindRow<FClothData>(Interface->GetClothDataTable(), Content.DataName, "Item Data Context."))
				{
					DefaultData.ClothData = (*ClothData);
					DefaultData.ItemCount = Content.DataCount;
					DefaultData.HandType = Content.HandType;

					Character->GetInventoryComponent()->AddItem(DefaultData, true);
				}
				//FClothData* ClothData = FindRow<FClothData>(Interface->GetClothDataTable(), Content.DataName, "Item Data Context.");
			}
			break;

			case EItemType::Enhancement:
			case EItemType::Arrow:
			case EItemType::Expendable:
			{
				if (FExpendableData* ExpendableData = FindRow<FExpendableData>(Interface->GetExpendableDataTable(), Content.DataName, "Item Data Context."))
				{
					if (ExpendableData->ExpendableType == EExpendableType::Potion || Content.DataCount > 99)
					{
						UE_LOG(LogSoulItem, Warning, TEXT("MatsterPickup = return nullptr"));
						return;// nullptr;
					}

					DefaultData.ExpendableData = (*ExpendableData);
					DefaultData.ItemCount = Content.DataCount;
					DefaultData.HandType = Content.HandType;

					Character->GetInventoryComponent()->AddItem(DefaultData, true);

				}
			}
			break;

			default:
				break;
			}
		}
	}
	//Character->Server_DestroyItem(this);
	Character->Server_RunInteraction(this, FVector::ZeroVector, FRotator::ZeroRotator);
}

FName AWorldPickupItem::GetInteractableName() const
{
	return Description;
}

void AWorldPickupItem::PlayInteractionMt()
{
	bOriginalCondition = false;
	SetActorHiddenInGame(true);
	Multicast_SetupCollision();
}

void AWorldPickupItem::Multicast_SetupCollision_Implementation()
{
	Collision->SetCollisionProfileName(TEXT("NoCollision"));
}


