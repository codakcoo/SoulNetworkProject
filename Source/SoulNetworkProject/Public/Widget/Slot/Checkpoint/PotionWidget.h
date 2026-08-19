// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PotionWidget.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class EPotionMenuType : uint8
 {
	Select		UMETA(DisplayName = "Select"),
	Add			UMETA(DisplayName = "Add"),
	Allocate	UMETA(DisplayName = "Allocate"),
	DoNot		UMETA(DisplayName = "DoNot")
 };


UCLASS()
class SOULNETWORKPROJECT_API UPotionWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION()
	EPotionMenuType GetMenuType() const { return MenuType; }
	UFUNCTION()
	void SetParentWidget(class UUserWidget* InWidget) { ParentWidget = InWidget; }
	UFUNCTION()
	void CallSelectMenu();

protected:
	//Menu Clicked//
	UFUNCTION()
	void AddClicked();
	UFUNCTION()
	void AllocateClicked();
	UFUNCTION()
	void BackClicked();
	////////////////

	//Add Clicked//
	UFUNCTION()
	void AddYesClicked();
	UFUNCTION()
	void AddNoClicked();
	//////////////

	//Allocate Clicked//
	UFUNCTION()
	void HealthClicked();
	UFUNCTION()
	void ManaClicked();
	UFUNCTION()
	void ApplyClicked();
	/////////////////////

protected:
	//Menu//
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* SelectMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* AddCountMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* AllocateMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* DoNotMenu;
	/////////

	//Menu Button//
	UPROPERTY(meta = (BindWidget))
	class UButton* AddMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* AllocateMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* BackMenuButton;
	///////////////

	//Add//
	UPROPERTY(meta = (BindWidget))
	class UButton* AddYesButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* AddNoButton;
	///////

	//Allocate//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ManaText;
	UPROPERTY(meta = (BindWidget))
	class UButton* HealthButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ManaButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* AllocateApplyButton;
	////////////

	//DoNot//
	UPROPERTY(meta = (BindWidget))
	class UButton* DoNotOKButton;
	////////

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* Blink;

private:
	class APlayerCharacter* Owner;

	uint8 MaxPotionCount = 0;
	uint8 HealthCount = 0;
	uint8 ManaCount = 0;

	EPotionMenuType MenuType;

	UUserWidget* ParentWidget;
};
