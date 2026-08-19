// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UI/TutorialWidget.h"

#include "Interface/InteractableInterface.h"

#include "Components/Button.h"

bool UTutorialWidget::SetupData(class AActor* InParent)
{
	if (!InParent)
	{
		RemoveFromViewport();
	}
	else
	{
		Parent = InParent;

		Setup();
		
		if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UTutorialWidget::CancelClicked);
		if (BGButton) BGButton->OnClicked.AddDynamic(this, &UTutorialWidget::CancelClicked);

		return true;
	}

	return false;
}

void UTutorialWidget::CancelClicked()
{
	if (Parent->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		Cast<IInteractableInterface>(Parent)->UpdatedOverlapEnd();
	}
}
