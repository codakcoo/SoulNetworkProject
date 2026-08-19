// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "OutofControlNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UOutofControlNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	//�ش��Ĺ���� Ʈ���̽� ��������(����)//
	UPROPERTY(EditAnywhere, Category = "Trace")
	FName F_SocketName = NAME_None;
	
	//�ش���Ͽ��� Ʈ���̽� ��������(����)//
	UPROPERTY(EditAnywhere, Category = "Trace")
	FName B_SocketName = NAME_None;
};
