// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/SmartObjectInteraction.h"
#include "ServerRowWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UServerRowWidget : public USmartObjectInteraction
{
	GENERATED_BODY()
	

public:
	UFUNCTION()
	virtual bool SetupData(class AActor* ParentActor) override;
		
	UFUNCTION()
	void OpenInteraction(class AActor* InParent, FText ServerName, uint8 CurrentPlayer, uint8 MaxPlayer, uint32 Index);

	UFUNCTION()
	void JoinClicked();
	UFUNCTION()
	void CancelClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InfoText;
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BGButton;

	TOptional<uint32> ServerIndex;

	class AActor* Parent = nullptr;
};
