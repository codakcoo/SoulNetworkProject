// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/CheckPoint.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/ObjectManager.h"

#include "Interface/CharacterInterface.h"
#include "Interface/GameStateInterface.h"
#include "Interface/SaveInterface.h"

#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/GameStateBase.h"

ACheckPoint::ACheckPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetupAttachment(GetRootComponent());
	Collision->SetSphereRadius(45.f);
	Collision->SetIsReplicated(true);


	Niagara = CreateDefaultSubobject<UNiagaraComponent>("Niagara");
	Niagara->SetupAttachment(Collision);
	Niagara->SetIsReplicated(true);
}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();

}

void ACheckPoint::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bPlayScale) UpdatedScaleParam(DeltaSeconds);
}

void ACheckPoint::Interact(APlayerCharacter* Character)
{
	if (Character)
	{
		//float StandardSize = Collision->GetScaledSphereRadius();
		//float RelativeSize = (Character->GetActorLocation() - GetActorLocation()).Size();
		//
		//float NormalSize = 0.4f + (RelativeSize / StandardSize);
		//
		//FVector LookLoc = 0.4f * (Character->GetActorLocation() - GetActorLocation()) + GetActorLocation();
		//FVector NewLoc(LookLoc.X, LookLoc.Y, Character->GetActorLocation().Z);
		
		if (bOriginalCondition)
		{
			FRotator NewRot(0.0f, UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), GetActorLocation()).Yaw, 0.0f);

			Character->Server_InteractionMontage(CheckInteractionMt, NAME_None);
			Character->Server_RunInteraction(this, FVector::ZeroVector, NewRot);

			FTimerHandle OverlapTimer;
			SetShapeComponent(Collision, TEXT("NoCollision"));
			GetWorld()->GetTimerManager().SetTimer(OverlapTimer, FTimerDelegate::CreateWeakLambda(this, [&]
			{
				SetShapeComponent(Collision, TEXT("SmartObject"));
			}), 1.0f, false);
		}
		else
		{
			Character->Server_InteractionMontage(RestInteractionMt, NAME_None);
			//Character->Server_RefreshManagers();
			//if(!Character->GetPlayerState()) return;
			//else if (Character->GetPlayerState()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
			//{
			//	Cast<ICharacterInterface>(Character->GetPlayerState())->UpdatedCheckPoint();
			//}
			if (!GetWorld()->GetGameState()) return;
			else if (GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UGameStateInterface::StaticClass()))
			{
				Cast<IGameStateInterface>(GetWorld()->GetGameState())->RefreshWorldData();
				Cast<ISaveInterface>(GetGameInstance())->SaveLastCheckPoint(PointName);
			}

		}
	}
}

FName ACheckPoint::GetInteractableName() const
{
	return bOriginalCondition ? TEXT("Activate point.") : TEXT("Rest at the point.");
}

void ACheckPoint::UpdatedOverlapEnd()
{
}

void ACheckPoint::PlayInteractionMt()
{
	if(bOriginalCondition)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CheckNSAsset, GetActorLocation());
		bOriginalCondition = false;
		Cast<ISaveInterface>(GetGameInstance())->SaveLastCheckPoint(PointName);
	}
	else
	{

	}
}

void ACheckPoint::CallEvent()
{
	SetupObject(true);
}

void ACheckPoint::SetupObject(bool bCondition)
{
	SetShapeComponent(Collision, bCondition ? TEXT("SmartObject") : TEXT("NoCollision"));
	bPlayScale = true;
	bActivate = bCondition;
}

void ACheckPoint::UpdatedScaleParam(float InDeltaTime)
{
	float Scale = GetFloatNiagaraParam(TEXT("Scale"));
	Scale = UKismetMathLibrary::FInterpTo(Scale, bActivate ? 1.0f : 0.0f, InDeltaTime, 10.f);
	Niagara->SetFloatParameter(TEXT("Scale"), Scale);

	if((bActivate && Scale == 1.0f) || (!bActivate && Scale == 0.0f)) bPlayScale = false;
}

float ACheckPoint::GetFloatNiagaraParam(FName VariableName)
{
	FNiagaraVariable NV(FNiagaraTypeDefinition::GetFloatDef(), VariableName);
	float OutValue;
	Niagara->GetOverrideParameters().GetParameterValue(OutValue, NV);

	return OutValue;
}
