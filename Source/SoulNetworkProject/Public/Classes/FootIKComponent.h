// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootIKComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULNETWORKPROJECT_API UFootIKComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFootIKComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
	void SetSocketName(FName InL_SocketName, FName InR_SocketName);

private:
	void Update_IK(float InDeltaTime);
	
	bool IsMoving();

	TTuple<bool, float> CapsuleDistance(FName InSocketName);

	TTuple<bool, float, FVector> FootLineTrace(FName InSocketName, ACharacter* InCharacter);
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	float IK_InterpSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	float L_Loc;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	float R_Loc;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	FRotator L_Rot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	FRotator R_Rot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	float HipOffset;

private:
	class ACharacter* Character;

	TArray<AActor*> IgnoreActors;

	FName L_SocketName;
	FName R_SocketName;

};
