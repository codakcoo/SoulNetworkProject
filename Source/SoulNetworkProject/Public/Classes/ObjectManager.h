// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Classes/SmartObject.h"
#include "ObjectManager.generated.h"

UCLASS()
class SOULNETWORKPROJECT_API AObjectManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void SaveWorldObject();
	UFUNCTION()
	void LoadWorldObject();


	UFUNCTION()
	void AllActiavateForAM();
	UFUNCTION()
	void RefreshManagers();
	UFUNCTION()
	void DeathEventBroadcastManager();

	UFUNCTION()
	void DeactivateInteractionObject();

	UFUNCTION()
	void OpenSessionBroadCast();
	UFUNCTION()
	void ActiveBossManagers();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	TArray<class AAIManager*> AIManagers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	TArray<class ABossManager*> BossManagers;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
	TArray<class ACheckPoint*> CheckPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
	TArray<class ASignPointActor*> SignPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
	TArray<class ASmartObject*> SmartObjects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TSubclassOf<class APickupActor> PickupClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TSubclassOf<class ARemainSoul> RemainSoulClass;

};
