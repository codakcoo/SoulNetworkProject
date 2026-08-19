// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Widget/MainMenu.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetServerNameToString(FServerData ServerData);

public:
	void Setup(class UMainMenu* InParent, uint32 InIndex);

public:
	UPROPERTY(BlueprintReadOnly)
	bool Selected = false;

private:
	UFUNCTION()
	void ServerOnClicked();

	UPROPERTY()
	UMainMenu* Parent;
	uint32 Index;

	UPROPERTY(meta = (BindWidget))
	class UButton* ServerButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostUser;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ConnectionFraction;
};
