// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ServerRow.h"


#include "Components/Button.h"

void UServerRow::SetServerNameToString(FServerData ServerData)
{
	ServerName->SetText(FText::FromString(ServerData.Name));
	HostUser->SetText(FText::FromString(ServerData.HostUser));
	ConnectionFraction->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"),ServerData.CurrentPlayers,ServerData.MaxPlayers)));
}

void UServerRow::Setup(class UMainMenu* InParent, uint32 InIndex)
{
	this->Parent = InParent;
	this->Index = InIndex;
	ServerButton->OnClicked.AddDynamic(this, &UServerRow::ServerOnClicked);
}

void UServerRow::ServerOnClicked()
{
	Parent->SelectIndex(Index);
}