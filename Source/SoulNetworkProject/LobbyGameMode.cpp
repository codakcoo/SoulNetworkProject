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
	bUseSeamlessTravel = true;	//트렌지션맵을 사용하는경우 다른맵으로 로드시에 트렌지션맵으로 먼저 이동한 다음 맵을 로드 시켜줍니다. bUseSeamlessTravel = true로 해줘야 이동이 가능하다.//
	GetWorld()->ServerTravel("/Game/Maps/ThirdPersonExampleMap?listen"); //서버의 합류한 클라이언트들은 서버에서 연결을 끊은다음 새맵으로 다시로드해서 프리징이 걸림//
}