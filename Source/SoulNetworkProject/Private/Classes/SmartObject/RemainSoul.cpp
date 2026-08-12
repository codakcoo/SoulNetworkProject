// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/SmartObject/RemainSoul.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"
#include "Classes/EffectComponent.h"

#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

ARemainSoul::ARemainSoul()
{
	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetupAttachment(Billboard);
	Collision->SetSphereRadius(45.f);
	Collision->SetRelativeLocation(FVector(0.0f));
	//Collision->OnComponentBeginOverlap.AddDynamic(this, &ARemainSoul::BeginOverlap);
	//Collision->OnComponentEndOverlap.AddDynamic(this, &ARemainSoul::EndOverlap);

}

void ARemainSoul::BeginPlay()
{
	Super::BeginPlay();
}

void ARemainSoul::Interact(APlayerCharacter* Character)
{
	if (Character && Character->GetEffectComponent() && Character->GetPC())
	{
		Character->AddSoul(Soul);
		Character->GetEffectComponent()->PlayEffect(AbsorptionSoundAsset, AbsorptionNiagaraAsset);
		Character->Server_RunInteraction(this, FVector::ZeroVector, FRotator::ZeroRotator);
		Character->GetPC()->Client_ClearLostSoul();
	}
}

FName ARemainSoul::GetInteractableName() const
{
	return TEXT("Gain Soul.");
}

void ARemainSoul::UpdatedOverlapEnd()
{
}

void ARemainSoul::PlayInteractionMt()
{
	Destroy();
}

void ARemainSoul::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, __FUNCTION__);
	}
}

void ARemainSoul::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, __FUNCTION__);
	}
}