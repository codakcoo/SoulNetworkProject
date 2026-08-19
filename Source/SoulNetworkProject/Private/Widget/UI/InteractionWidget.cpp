// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UI/InteractionWidget.h"
#include "Classes/Interaction/ServerRowActor.h"
#include "Interface/MenuInterface.h"
#include "Interface/InteractableInterface.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"


bool UInteractionWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	if (MM_HostButton) MM_HostButton->OnClicked.AddDynamic(this, &UInteractionWidget::MMHostClicked);
	if (MM_JoinButton) MM_JoinButton->OnClicked.AddDynamic(this, &UInteractionWidget::MMJoinClicked);
	if (HM_TeamupButton) HM_TeamupButton->OnClicked.AddDynamic(this, &UInteractionWidget::HMTeamupClicked);
	if (HM_InvasionButton) HM_InvasionButton->OnClicked.AddDynamic(this, &UInteractionWidget::HMInvasionClicked);

	if (MM_CancelButton && HM_CancelButton)
	{
		MM_CancelButton->OnClicked.AddDynamic(this, &UInteractionWidget::CancelClicked);
		HM_CancelButton->OnClicked.AddDynamic(this, &UInteractionWidget::CancelClicked);
	}

	if (YesButton) YesButton->OnClicked.AddDynamic(this, &UInteractionWidget::YesClicked);
	if (NoButton) NoButton->OnClicked.AddDynamic(this, &UInteractionWidget::CancelClicked);
	if (BGButton) BGButton->OnClicked.AddDynamic(this, &UInteractionWidget::CancelClicked);

	if (DescriptorText) DescriptorText->TextDelegate.BindDynamic(this, &UInteractionWidget::UpdatedDescriptorText);

	return true;
}


bool UInteractionWidget::SetupData(class AActor* InParent)
{
	if(!InParent)
	{
		RemoveFromViewport();
	}
	else if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		Parent = InParent;

		Setup();
		Cast<IMenuInterface>(GetGameInstance())->GetOpenSession() ? 
			MenuSwitch->SetActiveWidget(ConfirmBox) : 
			MenuSwitch->SetActiveWidget(MMSelectBox);

		PlayAnimation(BlinkUI);

		MMType = EMMType::None;
		HMType = EHMType::None;


		return true;
	}

	return false;
}

void UInteractionWidget::SetServerParent(AServerRowActor* InParent)
{
	if(!InParent) return;

	Parent = InParent;
}

void UInteractionWidget::MMHostClicked()
{
	PlayAnimation(BlinkUI);
	MenuSwitch->SetActiveWidget(HMSelectBox);
	HM_Text->SetText(FText::FromName(TEXT("Do select host mode")));
	MMType = EMMType::Host;
}

void UInteractionWidget::MMJoinClicked()
{

	PlayAnimation(BlinkUI);
	MenuSwitch->SetActiveWidget(HMSelectBox);
	HM_Text->SetText(FText::FromName(TEXT("Do select join mode")));
	MMType = EMMType::Join;
}

void UInteractionWidget::HMTeamupClicked()
{
	HMType = EHMType::Teamup;

	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()) &&
		Parent->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		FName SessionName;
		//SessionName = MMType == EMMType::Join ? TEXT("Join") : TEXT("Host");
		SessionName = TEXT("Teamup");

		MMType == EMMType::Join ?
			Cast<IMenuInterface>(GetGameInstance())->SearchServerList(SessionName) :
			Cast<IMenuInterface>(GetGameInstance())->Host(SessionName);


		Cast<IInteractableInterface>(Parent)->PlayInteractionMt();
		Cast<IInteractableInterface>(Parent)->UpdatedOverlapEnd();

	}
}

void UInteractionWidget::HMInvasionClicked()
{
	HMType = EHMType::Invasion;
	//if(!Parent) return;
	
	//if (Parent->ServerIndex.IsSet() && GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()) &&
		Parent->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		FName SessionName;
		//SessionName = MMType == EMMType::Join ? TEXT("Join") : TEXT("Host");
		SessionName = TEXT("Invasion");

		MMType == EMMType::Join ? 
			Cast<IMenuInterface>(GetGameInstance())->SearchServerList(SessionName) :
			Cast<IMenuInterface>(GetGameInstance())->Host(SessionName);
		//UE_LOG(LogTemp, Warning, TEXT("%d"), Parent->ServerIndex.GetValue());

		Cast<IInteractableInterface>(Parent)->PlayInteractionMt();
		Cast<IInteractableInterface>(Parent)->UpdatedOverlapEnd();
	}
}

void UInteractionWidget::CancelClicked()
{
	if(Parent->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		Cast<IInteractableInterface>(Parent)->UpdatedOverlapEnd();
	}
}

void UInteractionWidget::YesClicked()
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()) &&
		Parent->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		//Cast<IMenuInterface>(GetGameInstance())->SetSignPoint(nullptr);
		if (Cast<IMenuInterface>(GetGameInstance())->GetOpenSession())
		{
			if (Cast<IMenuInterface>(GetGameInstance())->GetStartSession())
			{
				Cast<IMenuInterface>(GetGameInstance())->Close();
				//Cast<IInteractableInterface>(Parent)->UpdatedOverlapEnd();
			}
			else
			{
				Cast<IMenuInterface>(GetGameInstance())->StartSession();
			}
			Cast<IInteractableInterface>(Parent)->UpdatedOverlapEnd();
		}
	}
}

FText UInteractionWidget::UpdatedDescriptorText()
{
	FString OutputText = TEXT("-");

	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		if (Cast<IMenuInterface>(GetGameInstance())->GetOpenSession())
		{
			if(Cast<IMenuInterface>(GetGameInstance())->GetStartSession()) OutputText = StartSessionDescriptor;
			else OutputText = OpenSessionDescriptor;
		}
	}
	
	return FText::FromString(OutputText);
}

