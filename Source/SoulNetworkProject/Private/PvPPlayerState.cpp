// Fill out your copyright notice in the Description page of Project Settings.


#include "PvPPlayerState.h"
#include "Classes/PlayerControllerBase.h"

void APvPPlayerState::AllPlayer_SendMessage_Implementation(FName Message)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
		APlayerControllerBase* TestPC = Cast<APlayerControllerBase>(*It);
		if (TestPC)
		{
			TestPC->BroadcastMessage(Message);

		}
	}
}