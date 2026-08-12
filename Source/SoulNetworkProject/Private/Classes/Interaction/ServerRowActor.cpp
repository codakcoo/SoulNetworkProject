// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Interaction/ServerRowActor.h"
#include "Classes/Interaction/SignPointActor.h"
#include "Classes/PlayerCharacter.h"
#include "Widget/UI/ServerRowWidget.h"
#include "Interface/MenuInterface.h"
#include "Widget/UI/InteractionWidget.h"


#include "Kismet/KismetMathLibrary.h"



AServerRowActor::AServerRowActor()
{
	SetReplicates(true);

	Description = TEXT("Activate a sign.");
}

void AServerRowActor::SetServerNameToString(FServerData ServerData)
{
	ServerName = FText::FromString(ServerData.HostUser);
	NumofCurrentPlayer = ServerData.CurrentPlayers;
	NumofMaxPlayer = ServerData.MaxPlayers;
}

void AServerRowActor::Setup(uint8 InIndex)
{
	ServerIndex = InIndex;
}

void AServerRowActor::Interact(APlayerCharacter* Character)
{
	//if (ServerIndex.IsSet() && GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	//{
	//	Cast<IMenuInterface>(GetGameInstance())->Join(ServerIndex.GetValue());
	//	UE_LOG(LogTemp, Warning, TEXT("%d"), ServerIndex.GetValue());
	//}
	if (Character && PopupWidget_C)
	{
		Character->Client_PopupMenu(this, PopupWidget_C, false);
		CurrentOwner = Character;

	}
	//CurrentWidget = CreateWidget<UServerRowWidget>(GetWorld(), PopupWidget_C);
	//if (CurrentWidget)
	//{
	//	//CurrentWidget->AddToViewport();
	//	//CurrentWidget->OpenInteraction(this, ServerName, 0, 4, (uint32)ServerIndex.GetValue());
	//
	//	CurrentOwner = Character;
	//}
}

FName AServerRowActor::GetInteractableName() const
{
	return Description;
}

void AServerRowActor::UpdatedOverlapEnd()
{
	if(CurrentOwner && CurrentOwner->GetHUD())
	{
		//CurrentWidget->Teardown();
		//CurrentWidget = nullptr;
		CurrentOwner->GetHUD()->ClearPopupWidget();
		CurrentOwner = nullptr;
	}
}

void AServerRowActor::PlayInteractionMt()
{
	//if (CurrentOwner)
	//{
	//	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(CurrentOwner->GetActorLocation(), GetActorLocation());
	//
	//	FRotator NewRot = FRotator(0.0f, LookRot.Yaw, 0.0f);
	//
	//	CurrentOwner->SetActorRotation(NewRot);
	//	CurrentOwner->Server_InteractionMontage(InteractionMt, NAME_None);
	//}
}
