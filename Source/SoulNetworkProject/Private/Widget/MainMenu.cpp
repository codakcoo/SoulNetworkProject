// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainMenu.h"
#include "Widget/OptionWidget.h"
#include "Widget/UI/CharacterSelectorWidget.h"
#include "Widget/UI/LoadGameWidget.h"
#include "SaveGame/SaveCharacterState.h"
#include "SaveGame/ClientInfo.h"
#include "Interface/MenuInterface.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "SoulNetworkLog.h"

#define RENDERONE TEXT("RENDERONE")
#define RENDERSEC TEXT("RENDERSEC")
#define RENDERTHI TEXT("RENDERTIR")
#define RENDERFOU TEXT("RENDERFOU")
#define RENDERFIV TEXT("RENDERFIV")

UMainMenu::UMainMenu(const FObjectInitializer& ObejectInitializer) : Super(ObejectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ServerRow_Class(TEXT("WidgetBlueprint'/Game/Widget/Server/ServerRow_BP.ServerRow_BP_C'"));
	if(ServerRow_Class.Succeeded()) ServerRowClass = ServerRow_Class.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> SaveRow_Class(TEXT("WidgetBlueprint'/Game/Widget/Game/SaveRow.SaveRow_C'"));
	if(SaveRow_Class.Succeeded()) SaveRowClass = SaveRow_Class.Class;
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	if(!OptionButton || !ExitButton) return false;// || !ServerJoinBackButton || !ServerJoinButton) return false;
	//if(JoinButton) JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if(NewGameButton) NewGameButton->OnClicked.AddDynamic(this, &UMainMenu::OpenNewGameMenu);
	if(LoadGameButton) LoadGameButton->OnClicked.AddDynamic(this, &UMainMenu::OpenLoadGameMenu);
	//if(HostButton) HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);
	if(OptionButton) OptionButton->OnClicked.AddDynamic(this, &UMainMenu::CallOption);
	if(ExitButton) ExitButton->OnClicked.AddDynamic(this, &UMainMenu::ExitProgram);

	//if(ServerJoinBackButton) ServerJoinBackButton->OnClicked.AddDynamic(this, &UMainMenu::CallMainMenu);
	//if(ServerJoinButton)ServerJoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);
	//
	//if(HostServerBackButton) HostServerBackButton->OnClicked.AddDynamic(this, &UMainMenu::CallMainMenu);
	//if(HostServerButton) HostServerButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);



	if(NewGameMenuWidget) NewGameMenuWidget->UpdatedPopup.AddDynamic(this, &UMainMenu::OnUpdatedPopup);
	if(LoadGameMenuWidget) LoadGameMenuWidget->UpdatedPopup.AddDynamic(this, &UMainMenu::OnUpdatedPopup);
	if(OptionMenuWidget) OptionMenuWidget->UpdatedPopup.AddDynamic(this, &UMainMenu::OnUpdatedPopup);
	//if(NewGameBackButton) NewGameBackButton->OnClicked.AddDynamic(this, &UMainMenu::CallMainMenu);
	//if(NewGameTextBox) NewGameTextBox->OnTextChanged.AddDynamic(this, &UMainMenu::UpdateEditTextBox);
	//if(NewGameCreateButton) NewGameCreateButton->OnClicked.AddDynamic(this, &UMainMenu::NewCreateSaveGame);

	return true;
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	//JoinButton->SetFocus();
	if(LoadGameMenuWidget) LoadGameMenuWidget->SetParentWidget(this);
	SetSaveList();
}

/*
void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	ServerList->ClearChildren();
	
	uint32 i = 0;
	for (const FServerData& ServerName : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(this, ServerRowClass);
		if (!Row) return;
		Row->SetServerNameToString(ServerName);

		ServerList->AddChild(Row);
		Row->Setup(this, i);
		i++;

	}
	UE_LOG(LogSoulUI, Warning, TEXT("Compelete ServerList"));
}
*/

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	//UpdateChildren();
}

void UMainMenu::SetSaveList()//TArray<USaveCharacterState*> SaveData)
{
	//LoadGameList->ClearChildren();
	//
	//UClientInfo* ClientData = nullptr;
	//if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	//{
	//	ClientData = Cast<IMenuInterface>(GetGameInstance())->GetClient();
	//	if(!ClientData) return;
	//
	//	int8 i = 0;
	//	for (FString& UserName : ClientData->SaveNames)
	//	{
	//		USaveRow* Row = CreateWidget<USaveRow>(this, SaveRowClass);
	//		if(!Row) return;
	//
	//		//UMaterialInterface* MatInterface = Cast<IMenuInterface>(GetGameInstance())->SetupCharacterImage(FName(*Data), i);
	//		//if(!MatInterface) continue;
	//
	//		UTexture2D* Texture = Cast<IMenuInterface>(GetGameInstance())->SetupCharacterImage(UserName, i);
	//		uint8 Level = Cast<IMenuInterface>(GetGameInstance())->GetUserLevel(UserName);
	//		Row->SetSaveData(FText::FromString(UserName), Level, Texture);
	//		LoadGameList->AddChild(Row);
	//		i++;
	//
	//
	//	}
	//
	//	NewGameButton->SetIsEnabled(i == 5 ? false : true);
	//	LoadGameButton->SetIsEnabled(i == 0 ? false : true);
	//	LoadGameText->SetText(FText::FromString(FString::FromInt(i)));
	//}

	if (LoadGameMenuWidget)
	{
		LoadGameMenuWidget->UpdatedSaveList();

		uint8 NumofSaveList = LoadGameMenuWidget->GetNumofSaveList();
		NewGameButton->SetIsEnabled(NumofSaveList == 5 ? false : true);
		LoadGameButton->SetIsEnabled(NumofSaveList == 0 ? false : true);
	}
}

void UMainMenu::ReverseMenu()
{
	if(MenuType == EMenuType_Main::MainMenu || bActivePopup) return;
	
	if(MenuType == EMenuType_Main::OptionMenu) OptionMenuWidget->ExitOption();
	else CallMainMenu();
}

void UMainMenu::SwitchMenu(bool bForward)
{
	if(MenuType != EMenuType_Main::OptionMenu || bActivePopup) return;

	OptionMenuWidget->SwitchingMenu(bForward);
}

//void UMainMenu::OpenJoinMenu()
//{
//	UE_LOG(LogTemp, Warning, TEXT("Call JoinServer"));
//
//	if(!MenuSwitcher || !ServerMenuWidget || !ServerJoinBackButton || !ServerJoinButton) return;
//
//	MenuSwitcher->SetActiveWidget(ServerMenuWidget);
//	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
//	{
//		IMenuInterface::Execute_RefreshServerList(GetGameInstance());
//	}
//}
//
//void UMainMenu::OpenHostMenu()
//{
//	if(MenuSwitcher && HostMenuWidget && HostServerBackButton && HostServerButton) MenuSwitcher->SetActiveWidget(HostMenuWidget);
//}

void UMainMenu::CallOption()
{
	if(!MenuSwitcher || !OptionMenuWidget) return;
	MenuSwitcher->SetActiveWidget(OptionMenuWidget);
	MenuType = EMenuType_Main::OptionMenu;

	if(UOptionWidget* OptionMenu = Cast<UOptionWidget>(OptionMenuWidget)) OptionMenu->SetParentWidget(this);
	UE_LOG(LogSoulUI, Warning, TEXT("OptionMenu Open"));
	PlayAnimation(OptionBlink);
}

//void UMainMenu::HostServer()
//{
//	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Call HostServer"));
//		Cast<IMenuInterface>(GetGameInstance())->Host(HostServerTextBox->GetText().ToString());
//	}
//}

void UMainMenu::ExitProgram()
{
	if(!GetWorld()) return;
	APlayerController* Con = GetWorld()->GetFirstPlayerController();
	if(!Con) return;

	Con->ConsoleCommand("EXIT");
}

void UMainMenu::OpenNewGameMenu()
{
	if(!NewGameMenuWidget) return;

	MenuSwitcher->SetActiveWidget(NewGameMenuWidget);
	MenuType = EMenuType_Main::NewMenu;

	//NewGameTextBox->SetText(FText::FromString(""));
	//UpdateEditTextBox(NewGameTextBox->GetText());
	PlayAnimation(NewBlink);
}

void UMainMenu::OpenLoadGameMenu()
{
	if(!LoadGameMenuWidget) return;

	MenuSwitcher->SetActiveWidget(LoadGameMenuWidget);
	MenuType = EMenuType_Main::LoadMenu;

	/*
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		Cast<IMenuInterface>(GetGameInstance())->RefreshSaveList();
	}
	*/
	
	PlayAnimation(LoadBlink);
}

void UMainMenu::CallMainMenu()
{
	if(MenuSwitcher && MainMenuWidget) MenuSwitcher->SetActiveWidget(MainMenuWidget);
	MenuType = EMenuType_Main::MainMenu;
	PlayAnimation(MainBlink);
}

void UMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet() && GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		UE_LOG(LogSoulUI,Warning, TEXT("Selected index %d"), SelectedIndex.GetValue());
		Cast<IMenuInterface>(GetGameInstance())->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogSoulUI, Warning, TEXT("Selected index not set"));
	}
}

void UMainMenu::UpdateEditTextBox(const FText& InText)
{
	FString ForLength = InText.ToString();


	//if(ForLength.Len() > 3 && !UGameplayStatics::DoesSaveGameExist(ForLength, 0)) NewGameCreateButton->SetIsEnabled(true);
	//else NewGameCreateButton->SetIsEnabled(false);
}

void UMainMenu::NewCreateSaveGame()
{
	UClientInfo* ClientData = nullptr;
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		ClientData = Cast<UClientInfo>(Cast<IMenuInterface>(GetGameInstance())->GetClient());
	}
	if(!ClientData) return;

	//ClientData->AddSaveData(NewGameTextBox->GetText());



	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		//Cast<IMenuInterface>(GetGameInstance())->AddSaveData(NewGameTextBox->GetText().ToString()); 
		//Cast<IMenuInterface>(GetGameInstance())->Host(NewGameTextBox->GetText().ToString());
		//Cast<IMenuInterface>(GetGameInstance())->SaveTravel();
	}
}

void UMainMenu::OnUpdatedPopup(bool bActive)
{
	bActivePopup = bActive;

}

//void UMainMenu::UpdateChildren()
//{
//	for (int32 i = 0; i < ServerList->GetChildrenCount(); i++)
//	{
//		auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));
//		if(Row) Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
//	}
//}
