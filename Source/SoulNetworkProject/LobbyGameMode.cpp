// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "SoulNetworkProjectGameInstance.h"


void ALobbyGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	bUseSeamlessTravel = true;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++NumberOfPlayers;
	if (NumberOfPlayers >= 2)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Reached 3 players!"));
		GetWorldTimerManager().SetTimer(InGameTimer, this, &ALobbyGameMode::StartGame, 10.f);
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--NumberOfPlayers;
}

void ALobbyGameMode::StartGame()
{
	auto GameInstance = Cast<USoulNetworkProjectGameInstance>(GetGameInstance());
	if(!GameInstance || !GetWorld()) return;
	
	GameInstance->StartSession();
	bUseSeamlessTravel = true;	//Ʈ�����Ǹ��� ����ϴ°�� �ٸ������� �ε�ÿ� Ʈ�����Ǹ����� ���� �̵��� ���� ���� �ε� �����ݴϴ�. bUseSeamlessTravel = true�� ����� �̵��� �����ϴ�.//
	GetWorld()->ServerTravel("/Game/Maps/ThirdPersonExampleMap?listen"); //������ �շ��� Ŭ���̾�Ʈ���� �������� ������ �������� �������� �ٽ÷ε��ؼ� ����¡�� �ɸ�//
}