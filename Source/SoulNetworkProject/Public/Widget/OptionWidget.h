// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/MenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Interface/SlateInterface.h"
#include "OptionWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatedOptionPopup, bool, bActive);

UENUM(BlueprintType)
enum class ESectionType : uint8
{
	GraphicSection UMETA(DisplayName = "Graphic"),
	AudioSection UMETA(DisplayName = "Sound"),
	GameplaySection UMETA(DisplayName = "Gameplay")
};

UCLASS()
class SOULNETWORKPROJECT_API UOptionWidget : public UUserWidget, public ISlateInterface
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	void SetParentWidget(UUserWidget* Parent) { ParentWidget = Cast<UMenuWidget>(Parent); }

	UFUNCTION()
	void ExitOption();

	UFUNCTION()
	void SwitchingMenu(bool bForward);

	//*******************Interface Function**********************//
	UFUNCTION()
	virtual void ConfirmPopup() override;
	UFUNCTION()
	virtual void CancelPopup() override;
	UFUNCTION()
	virtual void BackgroundPopup() override;
	//*****************************&*****************************//

private:
	UFUNCTION()
	void EnterGraphic();
	UFUNCTION()
	void EnterAudio();
	UFUNCTION()
	void EnterGameplay();

	UFUNCTION()
	void UpdatedSectionButton();

	UFUNCTION()
	bool GetGraphicEnabled() { return SectionType == ESectionType::GraphicSection ? false : true; }
	UFUNCTION()
	bool GetAudioEnabled() { return SectionType == ESectionType::AudioSection ? false : true; }
	UFUNCTION()
	bool GetGameplayEnabled() { return SectionType == ESectionType::GameplaySection ? false : true; }

	UFUNCTION()
	void EnterYesButton();
	UFUNCTION()
	void EnterNoButton();

public:
	UPROPERTY(BlueprintAssignable, Category = "Slot")
	FUpdatedOptionPopup UpdatedPopup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Popup")
	TSubclassOf<class UPopupWidget> PopupClass;

private:
	//Menu section enum//
	ESectionType SectionType = ESectionType::GraphicSection;
	////////////////////

	class UMenuWidget* ParentWidget = nullptr;

	//------------------Button Section---------------------//
	UPROPERTY(meta = (BindWidget))
	class UButton* GraphicButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* AudioButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* GameplayButton;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* BackEnterButton;

	//-----------------Menu Swithcer---------------//
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	//-----------------Detial Menu-----------------//
	UPROPERTY(meta = (BindWidget))
	class UWidget* GraphicMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* AudioMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* GameplayMenu;

	//-----------------Detial Menu-----------------//
	//UPROPERTY(meta = (BindWidget))
	//class UOverlay* ExitMenu;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* YesButton;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* NoButton;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* GraphicBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* AudioBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* GameplayBlink;

	class UPopupWidget* ConfirmWidget = nullptr;
	int8 MenuIndex = 0;
};
