// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/Interaction/SignPointActor.h"
#include "Classes/Interaction/ServerRowActor.h"
#include "Classes/SmartObject/SignPointBlock.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"
#include "Interface/MenuInterface.h"
#include "Widget/UI/InteractionWidget.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SoulNetworkLog.h"

ASignPointActor::ASignPointActor()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));

	//static ConstructorHelpers::FClassFinder<AServerRowActor> ServerClass_C(TEXT("Blueprint'/Game/BP/ServerRowActor_BP.ServerRowActor_BP_C'"));
	//if(ServerClass_C.Succeeded())
	//{
	//	ServerClass = ServerClass_C.Class;
	//}

	Description = TEXT("Activate a signpoint.");
}

void ASignPointActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASignPointActor::Interact(APlayerCharacter* Character)
{
	if(!Character) return;

	if (APlayerControllerBase* Con = Cast<APlayerControllerBase>(Character->GetController()))
	{
		//Con->ChangeActiveMenu();
		if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
		{
			if (PopupWidget_C)
			{
				//CurrnetWidget = CreateWidget<UInteractionWidget>(GetWorld(), PopupWidget_C);	
				//if (CurrnetWidget)
				//{
				//	//CurrnetWidget->AddToViewport();
				//	//CurrnetWidget->SetProperty(this);
				//}
				Character->Client_PopupMenu(this, PopupWidget_C, false);
				Cast<IMenuInterface>(GetGameInstance())->SetSignPoint(this);
				CurrentOwner = Character;

				//CurrentOwner->Client_Test(this);
			}

		}
	}
}

FName ASignPointActor::GetInteractableName() const
{
	return Description;
}

void ASignPointActor::UpdatedOverlapEnd()
{
	if(CurrentOwner && CurrentOwner->GetHUD())
	{
		//CurrnetWidget->Teardown();
		//CurrnetWidget = nullptr;
		CurrentOwner->GetHUD()->ClearPopupWidget();
		CurrentOwner = nullptr;
	}
}

void ASignPointActor::PlayInteractionMt()
{
	if (CurrentOwner)
	{
		FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(CurrentOwner->GetActorLocation(), GetActorLocation());
		FRotator NewRot = FRotator(0.0f, LookRot.Yaw, 0.0f);

		CurrentOwner->SetActorRotation(NewRot);
		CurrentOwner->Server_InteractionMontage(InteractionMt, NAME_None);

		DestroyServerRow();
	}
}

void ASignPointActor::SetServerRow(TArray<FServerData> ServerNames)
{
	UE_LOG(LogSoulGame, Warning, TEXT("Call ServerRow"));
	
	if (Searched_NS && Searched_SC)
	{
		Niagara->SetAsset(Searched_NS);
		Niagara->SetActive(true);
		Audio->SetSound(Searched_SC);
		Audio->Play();
	}

	uint8 i = 0;
	for (const FServerData& Server : ServerNames)
	{
		CreateServerRow(Server, i);
		i++;
	}
	/*
	*/
}

void ASignPointActor::CreateServerRow(const FServerData Data, uint8 Index)
{
	float SphereRadius = Collision->GetScaledSphereRadius();

	float RandomX = GetActorLocation().X + UKismetMathLibrary::RandomFloatInRange(UKismetMathLibrary::RandomFloatInRange(-SphereRadius, -SphereRadius - 30.f), UKismetMathLibrary::RandomFloatInRange(SphereRadius, SphereRadius + 30.f));
	float RandomY = GetActorLocation().Y + UKismetMathLibrary::RandomFloatInRange(UKismetMathLibrary::RandomFloatInRange(-SphereRadius, -SphereRadius - 30.f), UKismetMathLibrary::RandomFloatInRange(SphereRadius, SphereRadius + 30.f));
	FVector NewLocation = FVector(RandomX, RandomY, GetActorLocation().Z);

	FTransform SpawnTransform = FTransform(NewLocation);
	const FActorSpawnParameters SpawnParameters;
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);

	AServerRowActor* ServerRow = GetWorld()->SpawnActor<AServerRowActor>(ServerClass, SpawnTransform, SpawnParameters);

	ServerRow->SetServerNameToString(Data);
	ServerRow->Setup(Index);
	Servers.Add(ServerRow);
}

void ASignPointActor::DestroyServerRow()
{
	for (int8 i = 0; i < Servers.Num(); i++)
	{
		if(!Servers[i]) return;

		AServerRowActor* ServerRow = Cast<AServerRowActor>(Servers[i]);
		Servers[i]->Destroy();
		Servers.RemoveSingle(ServerRow);
	}
}

void ASignPointActor::SetupWalls(bool bActive)
{
	for (auto& Wall : Walls)
	{
		Wall->RefreshObject();
	}
}

void ASignPointActor::SwitchSearchEffect(bool bActive)
{
	if (Searching_NS && Searching_SC)
	{
		if (bActive)
		{
			Niagara->SetAsset(Searching_NS);
			Niagara->SetActive(true);
			Audio->SetSound(Searching_SC);
			Audio->Play();
		}
		else
		{
			Niagara->SetActive(false);
			Audio->Stop();
		}
	}
}

void ASignPointActor::SwitchSearchWall(bool bAcitve)
{
	for (auto& Wall : Walls)
	{
		Wall->SetSignPointWall(bAcitve);
	}
}

void ASignPointActor::RefreshObject(bool bAcitve)
{
	bOriginalCondition = bAcitve;

	Collision->SetCollisionProfileName(bOriginalCondition ? TEXT("SmartObject") : TEXT("NoCollision"));
}
