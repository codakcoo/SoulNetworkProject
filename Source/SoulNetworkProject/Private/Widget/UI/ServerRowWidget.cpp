// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UI/ServerRowWidget.h"
#include "Interface/MenuInterface.h"
#include "Interface/InteractableInterface.h"
#include "SoulNetworkProject/InGameModeBase.h"
#include "Classes/Interaction/ServerRowActor.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

bool UServerRowWidget::SetupData(class AActor* ParentActor)
{
	if (!ParentActor)
	{
		RemoveFromViewport();
	}
	else if(AServerRowActor* ServerActor = Cast<AServerRowActor>(ParentActor))
	{
		if(!ServerActor->GetServerIndex().IsSet()) return false;

		Parent = ParentActor;
		Setup();

		ServerIndex = ServerActor->GetServerIndex().GetValue();
		InfoText->SetText(ServerActor->GetServerName());

		//InfoText->SetText(ServerName);

		JoinButton->OnClicked.AddDynamic(this, &UServerRowWidget::JoinClicked);
		CancelButton->OnClicked.AddDynamic(this, &UServerRowWidget::CancelClicked);
		BGButton->OnClicked.AddDynamic(this, &UServerRowWidget::CancelClicked);

		return true;
	}
	return false;
}

void UServerRowWidget::OpenInteraction(AActor* InParent, FText ServerName, uint8 CurrentPlayer, uint8 MaxPlayer, uint32 Index)
{
	if (!InParent)
	{
		RemoveFromViewport();
		return;
	}

	Parent = InParent;

	Setup();

	ServerIndex = Index;

	InfoText->SetText(ServerName);

	JoinButton->OnClicked.AddDynamic(this, &UServerRowWidget::JoinClicked);
	CancelButton->OnClicked.AddDynamic(this, &UServerRowWidget::CancelClicked);
	BGButton->OnClicked.AddDynamic(this, &UServerRowWidget::CancelClicked);
}

void UServerRowWidget::JoinClicked()
{
	if (ServerIndex.IsSet() && GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()) &&
		Parent->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		Cast<IInteractableInterface>(Parent)->PlayInteractionMt();
		Cast<IInteractableInterface>(Parent)->UpdatedOverlapEnd();


		if (AInGameModeBase* GM = Cast<AInGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			GM->AllPlayer_SendMessage(TEXT("Entering the server."));
		}

		//FTimerHandle JoinTimer;
		//GetWorld()->GetTimerManager().SetTimer(JoinTimer, FTimerDelegate::CreateWeakLambda(this, [&]
		//{
		//	GetWorld()->GetTimerManager().ClearTimer(JoinTimer);
		//}), 5.0f, false);
			Cast<IMenuInterface>(GetGameInstance())->Join((uint32)ServerIndex.GetValue());
	}
}

void UServerRowWidget::CancelClicked()
{
	if (Parent->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		Cast<IInteractableInterface>(Parent)->UpdatedOverlapEnd();
	}
}
