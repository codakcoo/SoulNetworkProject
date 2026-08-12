// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulNetworkProject/ItemData.h"
#include "ClothComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULNETWORKPROJECT_API UClothComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClothComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void AddBodySlot(FItemData Item, FClothData Cloth);
	UFUNCTION(Server, Reliable)
	void Server_AddBodySlot(FItemData Item, FClothData Cloth);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AddBodySlot(FItemData Item, FClothData Cloth);

	UFUNCTION()
	void RemoveBodySlot(FItemData Item, FClothData Cloth);
	UFUNCTION(Server, Reliable)
	void Server_RemoveBodySlot(FItemData Item, FClothData Cloth);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RemoveBodySlot(FItemData Item, FClothData Cloth);

	UFUNCTION()
	void RefreshClothMesh();

	UFUNCTION()
	void BodySlot(FItemData ItemData, FClothData ClothData, bool bActive);

	UFUNCTION()
	void AddClothMesh(FItemData ItemData, FClothData ClothData);

private:
	UPROPERTY()
	class APlayerCharacter* Owner;
};
