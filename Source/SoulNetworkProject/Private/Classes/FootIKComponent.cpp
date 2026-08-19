// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/FootIKComponent.h"

#include "Gameframework/Character.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UFootIKComponent::UFootIKComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFootIKComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	Character = Cast<ACharacter>(GetOwner());
	if(!Character) return;
	
	IgnoreActors.Add(GetOwner());
}


// Called every frame
void UFootIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	Update_IK(DeltaTime);
}

void UFootIKComponent::SetSocketName(FName InL_SocketName, FName InR_SocketName)
{
	L_SocketName = InL_SocketName;
	R_SocketName = InR_SocketName;
}

void UFootIKComponent::Update_IK(float InDeltaTime)
{
	if (IsMoving())
	{
		TTuple<bool, float> L_Foot = CapsuleDistance(L_SocketName);
		TTuple<bool, float> R_Foot = CapsuleDistance(R_SocketName);

		if (L_Foot.Get<0>() || L_Foot.Get<0>())
		{
			const float Selectfloat = UKismetMathLibrary::SelectFloat(L_Foot.Get<1>(), R_Foot.Get<1>(), L_Foot.Get<1>() >= R_Foot.Get<1>());
			HipOffset = FMath::FInterpTo(HipOffset,(Selectfloat - 98.f) * -1.f, InDeltaTime, IK_InterpSpeed);

			TTuple<bool, float, FVector> L_FootTrace = FootLineTrace(L_SocketName, Character);
			TTuple<bool, float, FVector> R_FootTrace = FootLineTrace(R_SocketName, Character);

			const float L_Distance = L_FootTrace.Get<1>();
			const FVector L_Vector = L_FootTrace.Get<2>();
			const FRotator MakeL_Rot(UKismetMathLibrary::DegAtan2(L_Vector.X, L_Vector.Z) * -1.f, 0.f, UKismetMathLibrary::DegAtan2(L_Vector.Y,L_Vector.Z));

			L_Rot = FMath::RInterpTo(L_Rot, MakeL_Rot, InDeltaTime, IK_InterpSpeed);
			L_Loc = FMath::FInterpTo(L_Loc, (L_Distance - 110.f) / -45.f, InDeltaTime, IK_InterpSpeed);

			const float R_Distance = R_FootTrace.Get<1>();
			const FVector R_Vector = R_FootTrace.Get<2>();
			const FRotator MakeR_Rot(UKismetMathLibrary::DegAtan2(R_Vector.X, R_Vector.Z) * -1.f, 0.f, UKismetMathLibrary::DegAtan2(R_Vector.Y, R_Vector.Z));

			R_Rot = FMath::RInterpTo(R_Rot, MakeR_Rot, InDeltaTime, IK_InterpSpeed);
			R_Loc = FMath::FInterpTo(R_Loc, (R_Distance - 110.f) / -45.f, InDeltaTime, IK_InterpSpeed);

		}
		else
		{
			L_Rot = FMath::RInterpTo(L_Rot, FRotator::ZeroRotator, InDeltaTime, IK_InterpSpeed);
			L_Loc = FMath::FInterpTo(L_Loc, 0.f, InDeltaTime, IK_InterpSpeed);

			R_Rot = FMath::RInterpTo(R_Rot, FRotator::ZeroRotator, InDeltaTime, IK_InterpSpeed);
			R_Loc = FMath::FInterpTo(R_Loc, 0.f, InDeltaTime, IK_InterpSpeed);
		}
	}
}

bool UFootIKComponent::IsMoving()
{
	float Speed = Character->GetVelocity().Size();
	
	if(Speed <= 0.f) return true;
	if(!Character->GetMovementComponent()->IsFalling()) return true;

	return false;
}

TTuple<bool, float> UFootIKComponent::CapsuleDistance(FName InSocketName)
{
	const FVector WorldLocation = Character->GetMesh()->GetComponentLocation();
	const FVector BreakVector = WorldLocation+FVector(0.f, 0.f, 98.f);

	const FVector SocketLocation  = Character->GetMesh()->GetSocketLocation(InSocketName);
	
	const FVector Start = FVector(SocketLocation.X, SocketLocation.Y, BreakVector.Z);
	const FVector End = Start - FVector(0.f, 0.f, 151.f);

	FHitResult OutHit;

	UKismetSystemLibrary::LineTraceSingle(this, Start, End, TraceTypeQuery5, false, IgnoreActors, EDrawDebugTrace::None, OutHit, false);

	const bool Result = OutHit.bBlockingHit;

	return MakeTuple(Result, OutHit.Distance);
}

TTuple<bool, float, FVector> UFootIKComponent::FootLineTrace(FName InSocketName, ACharacter* InCharacter)
{
	const FVector SocketLocation = Character->GetMesh()->GetSocketLocation(InSocketName);
	const FVector RootLocation = Character->GetMesh()->GetSocketLocation("root");

	const FVector Start = FVector(SocketLocation.X, SocketLocation.Y, RootLocation.Z);

	FHitResult OutHit;

	UKismetSystemLibrary::LineTraceSingle(this, Start + FVector(0.f, 0.f, 105.f), Start + FVector(0.f, 0.f, -105.f), TraceTypeQuery5, false, IgnoreActors, EDrawDebugTrace::None, OutHit, false);

	const bool Result = OutHit.bBlockingHit;

	if(OutHit.bBlockingHit) return MakeTuple(Result, OutHit.Distance, OutHit.Normal);
	else return MakeTuple(Result, 999.f, FVector::ZeroVector);
}

