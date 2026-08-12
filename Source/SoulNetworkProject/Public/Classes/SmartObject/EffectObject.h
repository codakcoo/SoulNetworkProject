// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/SmartObject.h"
#include "EffectObject.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
 enum class EEffectObjectType : uint8
 {
	OnlyScale		UMETA(DisplayName = "OnlyScale"),
	ScaleAndBeamEnd	UMETA(DisplayName = "ScaleAndBeamEnd")
 };

UCLASS()
class SOULNETWORKPROJECT_API AEffectObject : public ASmartObject
{
	GENERATED_BODY()
	
protected:
	AEffectObject();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

public:
	UFUNCTION()
	virtual void SetupObject(bool bCondition) override;
	UFUNCTION()
	void SetEffectOwner(class ACharacterBase* InActor) { EffectOwner = InActor; }

	UFUNCTION(BlueprintCallable)
	void SetActivate(bool bActivate);
	UFUNCTION(BlueprintCallable)
	void RunEffect();

	UFUNCTION()
	void SetEffectObject(bool bActivate);

private:
	UFUNCTION()
	void SetScaleParam(bool bActivate);
	UFUNCTION()
	void SetBeamParam(bool bActivate);

	UFUNCTION()
	float GetFloatNiagaraParam(FName VariableName);
	UFUNCTION()
	FVector GetVectortNiagaraParam(FName VariableName);

	UFUNCTION()
	void UpdatedScaleParam(float InDeltaTime);
	UFUNCTION()
	void UpdatedBeamEndAParam(float InDeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UNiagaraComponent* Niagara;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	class UAudioComponent* Audio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	class USoundBase* AdditionalCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	class USoundBase* ScaleCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	class USoundBase* BeamdCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	float AddtionalSoundValue = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
	EEffectObjectType ObjectType = EEffectObjectType::OnlyScale;

private:
	class ACharacterBase* EffectOwner;

	bool bScale = false;
	bool bScaleForward = false;
	bool bBeamEnd = false;
};
