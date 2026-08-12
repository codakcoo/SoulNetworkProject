// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/ClientInfo.h"
#include "SaveGame/SaveCharacterState.h"

#include "Kismet/GameplayStatics.h"

#define CLIENT_NAME FName("Client").ToString()

void UClientInfo::UpdateOptionValue()
{
	
}

bool UClientInfo::AddSavename(FString SaveName)
{
	if(SaveNames.Num() > 4) return false;

	SaveNames.Add(SaveName);
	UGameplayStatics::SaveGameToSlot(this, CLIENT_NAME, 0);

	return true;
}

bool UClientInfo::RemoveSaveName(FString SaveName)
{
	//if(SaveNames.Num() < 0) return false;

	SaveNames.RemoveSingle(SaveName);
	UGameplayStatics::SaveGameToSlot(this, CLIENT_NAME, 0);

	return true;
}
