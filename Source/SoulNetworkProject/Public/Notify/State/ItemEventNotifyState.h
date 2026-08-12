// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ItemEventNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UItemEventNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Event|Mesh")
	class UStaticMesh* Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Event|Mesh")
	FVector Scale;
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Event|Mesh")
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Event|Niagara")
	class UNiagaraSystem* Niagara;

};
