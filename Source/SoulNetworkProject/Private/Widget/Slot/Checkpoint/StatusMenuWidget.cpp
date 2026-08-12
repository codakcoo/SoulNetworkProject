// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Slot/Checkpoint/StatusMenuWidget.h"

UStatusMenuWidget::UStatusMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UStatusMenuWidget::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	return true;
}

void UStatusMenuWidget::NativeConstruct()
{
}
