// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/EffectObject.h"
#include "Classes/CharacterBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"

#include "Kismet/GameplayStatics.h"

#define ACTIVATE TEXT("ACTIVATE")

AEffectObject::AEffectObject()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Billboard);
	Mesh->SetIsReplicated(true);

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(Billboard);
	Niagara->SetIsReplicated(true);

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(Billboard);
	Audio->SetIsReplicated(true);


	SetReplicates(true);
}

void AEffectObject::BeginPlay()
{
	Super::BeginPlay();

	//FTimerHandle TurnOnTimer;
	//GetWorld()->GetTimerManager().SetTimer(TurnOnTimer, FTimerDelegate::CreateWeakLambda(this, [&]()
	//{
	//	SetupObject(true);
	//}), 5.0f, false);
	Niagara->SetVectorParameter(TEXT("BeamEnd"), Niagara->GetComponentLocation());
	SetActivate(bOriginalCondition);
}
void AEffectObject::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bScale)
	{
		UpdatedScaleParam(DeltaSeconds);
		float CurrentScale = GetFloatNiagaraParam(TEXT("Scale"));
		if((bScaleForward && CurrentScale >= 1.0f) || (!bScaleForward && CurrentScale <= 0.0f)) bScale = false;
	}
	if(bBeamEnd) UpdatedBeamEndAParam(DeltaSeconds);
}

void AEffectObject::SetupObject(bool bCondition)
{
	bOriginalCondition = bCondition;
	SetActivate(bOriginalCondition);
}

void AEffectObject::SetActivate(bool bActivate)
{
	bOriginalCondition = bActivate;
	TArray<UActorComponent*> OutComponents = GetComponentsByTag(UActorComponent::StaticClass(), ACTIVATE);
	for (UActorComponent* OutComponent : OutComponents)
	{
		if (USceneComponent* SceneComonent = Cast<USceneComponent>(OutComponent)) SceneComonent->SetVisibility(bActivate);
	}
	bActivate ? Audio->Play() : Audio->Stop();
}

void AEffectObject::RunEffect()
{
	SetActivate(true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), AdditionalCue, GetActorLocation(), AddtionalSoundValue);
}

void AEffectObject::SetEffectObject(bool bActivate)
{
	if (ObjectType == EEffectObjectType::OnlyScale) SetScaleParam(bActivate);
	else if (ObjectType == EEffectObjectType::ScaleAndBeamEnd)
	{
		SetScaleParam(bActivate);
		SetBeamParam(bActivate);
	}
}

void AEffectObject::SetScaleParam(bool bActivate)
{
	bScale = true;
	bScaleForward = bActivate;
	bActivate ? Audio->Play() : Audio->Stop();
	if (bActivate) 	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ScaleCue, GetActorLocation(), 1.0f);
}

void AEffectObject::SetBeamParam(bool bActivate)
{
	bBeamEnd = bActivate;
	if(bBeamEnd) Niagara->SetVectorParameter(TEXT("BeamEnd"), Niagara->GetComponentLocation());
	if (bActivate) UGameplayStatics::PlaySoundAtLocation(GetWorld(), BeamdCue, GetActorLocation(), 1.0f);
}

float AEffectObject::GetFloatNiagaraParam(FName VariableName)
{
	FNiagaraVariable NV(FNiagaraTypeDefinition::GetFloatDef(), VariableName);
	float OutValue;
	Niagara->GetOverrideParameters().GetParameterValue(OutValue, NV);

	return OutValue;
}
FVector AEffectObject::GetVectortNiagaraParam(FName VariableName)
{
	FNiagaraVariable NV(FNiagaraTypeDefinition::GetVec3Def(), VariableName);
	FVector OutValue = FVector::ZeroVector;
	Niagara->GetOverrideParameters().GetParameterValue(OutValue, NV);

	return OutValue;
}

void AEffectObject::UpdatedScaleParam(float InDeltaTime)
{
	if (Niagara)
	{
		float CurrentScale = GetFloatNiagaraParam(TEXT("Scale"));
		float LerpScale = FMath::FInterpTo(CurrentScale, bScaleForward ? 1.0f : 0.0f, InDeltaTime, 10.0f);
		Niagara->SetFloatParameter(TEXT("Scale"), LerpScale);
	}
}

void AEffectObject::UpdatedBeamEndAParam(float InDeltaTime)
{
	if (Niagara && EffectOwner)
	{
		FVector CurrentVector = GetVectortNiagaraParam(TEXT("BeamEnd"));
		FVector SocketVector = EffectOwner->GetMainWeaponMesh()->GetSocketLocation(TEXT("Enchant"));
		FVector LerpVector = FMath::VInterpTo(CurrentVector, SocketVector, InDeltaTime, 20.0f);
		Niagara->SetVectorParameter(TEXT("BeamEnd"), LerpVector);
	}
}
