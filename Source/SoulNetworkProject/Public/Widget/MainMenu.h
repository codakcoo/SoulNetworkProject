// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "Interface/MenuInterface.h"
#include "Classes/Interaction/SignPointActor.h"
#include "MainMenu.generated.h"


UENUM(BlueprintType)
enum class EMenuType_Main : uint8
{
	MainMenu UMETA(DisplayName = "MainMenu"),
	NewMenu UMETA(DisplayName = "NewMenu"),
	LoadMenu UMETA(DisplayName = "LoadMenu"),
	OptionMenu UMETA(DisplayName = "OptionMenu")
};

USTRUCT()
struct FTestServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUser;
};

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
	
public:
	UMainMenu(const FObjectInitializer& ObejectInitializer);
	
protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	virtual void CallbackMenu() override { CallMainMenu(); }

	//void SetServerList(TArray<FServerData> ServerNames);

	void SelectIndex(uint32 Index);

	UFUNCTION()
	void SetSaveList();

	UFUNCTION(BlueprintCallable)
	void ReverseMenu();

	UFUNCTION(BlueprintCallable)
	virtual void SwitchMenu(bool bForward);

	UFUNCTION()
	void CallMainMenu();
private:
	TSubclassOf<class UUserWidget> ServerRowClass;
	TSubclassOf<class UUserWidget> SaveRowClass;

	//UFUNCTION()
	//void OpenJoinMenu();
	//UFUNCTION()
	//void OpenHostMenu();
	UFUNCTION()
	void CallOption();
	//UFUNCTION()
	//void HostServer();
	UFUNCTION()
	void ExitProgram();

	UFUNCTION()
	void OpenNewGameMenu();
	UFUNCTION()
	void OpenLoadGameMenu();


	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void UpdateEditTextBox(const FText& InText);
	UFUNCTION()
	void NewCreateSaveGame();

	UFUNCTION()
	void OnUpdatedPopup(bool bActive);

	//UFUNCTION()
	//void UpdateChildren();

private:
	//MainMenu//
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenuWidget;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* JoinButton;

	//NewGameButton//
	UPROPERTY(meta = (BindWidget))
	class UButton* NewGameButton;
	//LoadGameButton//
	UPROPERTY(meta = (BindWidget))
	class UButton* LoadGameButton;
	/// //////////
	UPROPERTY(meta = (BindWidget))
	class UButton* OptionButton;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;
	////////////

	////ServerMenu//
	//UPROPERTY(meta = (BindWidget))
	//class UWidget* ServerMenuWidget;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* ServerJoinBackButton;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* ServerJoinButton;
	//UPROPERTY(meta = (BindWidget))
	//class UPanelWidget* ServerList;
	///////////////

	//NewGame//
	UPROPERTY(meta = (BindWidget))
	class UCharacterSelectorWidget* NewGameMenuWidget;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* NewGameBackButton;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* NewGameCreateButton;
	//UPROPERTY(meta = (BindWidget))
	//class UEditableTextBox* NewGameTextBox;

	//LoadGame//
	UPROPERTY(meta = (BindWidget))
	class ULoadGameWidget* LoadGameMenuWidget;

	//UPROPERTY(meta = (BindWidget))
	//class UWidget* LoadRow_1;
	//UPROPERTY(meta = (BindWidget))
	//class UWidget* LoadRow_2;
	//UPROPERTY(meta = (BindWidget))
	//class UWidget* LoadRow_3;
	//UPROPERTY(meta = (BindWidget))
	//class UWidget* LoadRow_4;
	//UPROPERTY(meta = (BindWidget))
	//class UWidget* LoadRow_5;
	////////////

	//OptionMenu//
	UPROPERTY(meta = (BindWidget))
	class UOptionWidget* OptionMenuWidget;
	//////////////

	////HostMenu//
	//UPROPERTY(meta = (BindWidget))
	//class UWidget* HostMenuWidget;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* HostServerBackButton;
	//UPROPERTY(meta = (BindWidget))
	//class UButton* HostServerButton;
	//UPROPERTY(meta = (BindWidget))
	//class UEditableTextBox* HostServerTextBox;
	//////////////

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	TOptional<uint32> SelectedIndex;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* MainBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* NewBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* LoadBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OptionBlink;

	EMenuType_Main MenuType = EMenuType_Main::MainMenu;
	bool bActivePopup = false;
};
