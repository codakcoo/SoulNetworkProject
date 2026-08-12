// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SmartObject.generated.h"

#define SMARTOBJECT TEXT("SmartObject")
#define NOCOLLISION TEXT("NoCollision")

UCLASS()
class SOULNETWORKPROJECT_API ASmartObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmartObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UFUNCTION()
	void SetOriginalCondition(bool bActivate);
	UFUNCTION(Server, Reliable)
	void Server_SetOriginalCondition(bool bActivate);
	UFUNCTION()
	bool GetOriginalCondition() const { return bOriginalCondition; }

	virtual void SetupObject(bool bCondition) {}

	UFUNCTION()
	void SetShapeComponent(class UShapeComponent* CollisionComp, FName CollisionName);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UBillboardComponent* Billboard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* FacingDirection;
	


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* SubTree;

protected:
	UPROPERTY(Replicated)
	bool bOriginalCondition = true;

};
