// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/TutorialArea.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"
#include "Components/BillboardComponent.h"
#include "Widget/UI/InteractionWidget.h"
#include "SoulNetworkProject/PlayerHUD.h"

#include "Interface/MenuInterface.h"

#include "Components/BoxComponent.h"

ATutorialArea::ATutorialArea()
{
	TriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	TriggerArea->SetupAttachment(Billboard);
	TriggerArea->SetIsReplicated(true);
	TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &ATutorialArea::BeginOverlap);
}

void ATutorialArea::BeginPlay()
{
	Super::BeginPlay();

	UpdateTriggerBox();
}

void ATutorialArea::Interact(APlayerCharacter* Character)
{
	if (!Character || PopupWidgets_C.Num() == 0) return;
	if (CurrentOwner) return;                    // 이미 다른 플레이어가 보는 중

	WidgetIndex = 0;
	CurrentOwner = Character;
	bOriginalCondition = false;
	UpdateTriggerBox();                          // ← 콜리전 실제로 끄기

	CurrentOwner->Client_PopupMenu(this, PopupWidgets_C[WidgetIndex], true);
}

FName ATutorialArea::GetInteractableName() const
{
	return TEXT("");
}

void ATutorialArea::UpdatedOverlapEnd()
{
	if (CurrentOwner && CurrentOwner->GetHUD())
	{
		CurrentOwner->GetHUD()->ClearPopupWidget();
		WidgetIndex++;

		if(PopupWidgets_C.Num() != WidgetIndex) CurrentOwner->Client_PopupMenu(this, PopupWidgets_C[WidgetIndex], true);
		else
		{
			CurrentOwner->Client_PopupMenu(nullptr, nullptr, false);
			CurrentOwner = nullptr;
			UpdateTriggerBox();
		}

	}
}

void ATutorialArea::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if(TutorialTrigger == ETutorialTrigger::End) return;

	if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
	{
		if (player->GetLocalRole() == ROLE_Authority)
		{
			Interact(player);
		}
	}
}


void ATutorialArea::UpdateTriggerBox()
{
	SetShapeComponent(TriggerArea, bOriginalCondition ? TEXT("OverlapAllDynamic") : TEXT("NoCollision"));
}
