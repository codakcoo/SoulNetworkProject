// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/ProjectileComponent.h"
#include "Classes/CharacterBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"

// Sets default values for this component's properties
UProjectileComponent::UProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	UpdatedProjectile(DeltaTime);
}

void UProjectileComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UProjectileComponent, bIsBulletInitialized);
	DOREPLIFETIME(UProjectileComponent, SpawnOwner);
	DOREPLIFETIME(UProjectileComponent, TrackChr);
	DOREPLIFETIME(UProjectileComponent, TrackBoneName);
}

void UProjectileComponent::RunProjectile(ACharacterBase* InSpawnOwner, ACharacterBase* InTrackChr, FName InTrackBoneName)
{
		
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* CO = GetOwner();
		if (!CO) return;

		//Test Method//
		//float NewVelocitySize = 0.0f;
		//if(InTrackChr && InSpawnOwner) NewVelocitySize = (InTrackChr->GetActorLocation() - CO->GetActorLocation()).Size();
		//GEngine->AddOnScreenDebugMessage(-1,10.f, FColor::White,FString::Printf(TEXT("%f"), NewVelocitySize));
		//Velocity = CO->GetActorForwardVector() * NewVelocitySize;
		if(AddRandSpeed > 0.0f) Speed += FMath::RandRange(0.0f, AddRandSpeed);

		Velocity = CO->GetActorForwardVector() * Speed;
		StartLocation = CO->GetActorLocation();
		bIsBulletInitialized = true;

		SpawnOwner = InSpawnOwner;
		TrackChr = InTrackChr;
		TrackBoneName = InTrackBoneName;

		if (SoulDebug::ShouldDraw(bDrawDebug)) DrawProjectileDebug();
	}
	else
	{
		Server_RunProjectile(InSpawnOwner, InTrackChr, InTrackBoneName);
	}
}

void UProjectileComponent::Server_RunProjectile_Implementation(ACharacterBase* InSpawnOwner, ACharacterBase* InTrackChr, FName InTrackBoneName)
{
	RunProjectile(InSpawnOwner, InTrackChr, InTrackBoneName);
}

void UProjectileComponent::StopProjectile()
{
	if (!GetOwner()) return;

	bIsBulletInitialized = false;
}

void UProjectileComponent::UpdatedProjectile(float InDeltaTime)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* CO = GetOwner();
		if (bIsBulletInitialized && CO)
		{
			StartLocation = CO->GetActorLocation();
			if (LoopType == ELoopType::Projectile)
			{

				FVector NextLocation = StartLocation + (Velocity * InDeltaTime);
				FRotator DesiredRotation = Velocity.Rotation();
				if (bUseApplyRotation)
				{
					DesiredRotation.Pitch = FRotator::NormalizeAxis(DesiredRotation.Pitch);
					DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
					DesiredRotation.Roll = 0.0f;
					CO->GetRootComponent()->SetWorldRotation(DesiredRotation);
				}


				CO->SetActorLocation(NextLocation);

				Velocity = CalculateGravityAndDecelaration(Velocity, InDeltaTime);
			}

			//FVector CurrentVector = GetActorLocation();
			////FVector MovementVector = TrackChr ? TrackChr->GetActorLocation() : CurrentVector + GetActorForwardVector() * 1000.f;
			//FVector MovementVector = CurrentVector + GetActorForwardVector() * 100.0f;

			//CurrentVector = UKismetMathLibrary::VInterpTo(CurrentVector, MovementVector, InDeltaTime, ProjectileSpeed);
			//if (!bStopMovement) SetActorLocation(CurrentVector);
			if (LoopType == ELoopType::Homing)
			{
			
				Velocity = CO->GetActorForwardVector() * Speed;
				FVector NexeLocation = StartLocation + (Velocity * InDeltaTime);
				CO->SetActorLocation(NexeLocation);

				if (TrackChr)
				{
					float CurrentDistance = FMath::Abs((CO->GetActorLocation() - SpawnOwner->GetActorLocation()).Size());
					float TargetDistance = FMath::Abs((TrackChr->GetActorLocation() - SpawnOwner->GetActorLocation()).Size());

					if (CurrentDistance < TargetDistance)
					{

						FRotator CurrentRot = CO->GetActorRotation();
						FVector OriginLoc = CO->GetActorLocation();
						FVector TargetLoc = TrackBoneName == NAME_None ? TrackChr->GetActorLocation() : TrackChr->GetMesh()->GetSocketLocation(TrackBoneName);

						//float CurrentDeltaYaw =  UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TrackChr->GetActorLocation()).Yaw;
						//FRotator NewRot = FRotator(CurrentRot.Pitch, CurrentDeltaYaw, CurrentRot.Roll);

						FRotator DeltaRotation = UKismetMathLibrary::FindLookAtRotation(CO->GetActorLocation(), TargetLoc);
						CurrentRot = UKismetMathLibrary::RInterpTo(CurrentRot, DeltaRotation, InDeltaTime, HomingSpeed);
 
						//if (!bStopMovement) 
						//Multicast_UpdatedProjectile(CurrentRot);
						//CO->SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
						//CO->AddActorWorldRotation(FQuat(-10.0f, -10.0f, 0.0f, 0.0f));
						//
						////FVector NextLocation = StartLocation + (TargetLoc * InDeltaTime);
						//FVector NextLocation = UKismetMathLibrary::VInterpTo(StartLocation, TargetLoc, InDeltaTime, HomingSpeed);
						//SetWorldRotation()
						SoulDebug::ScreenLog(TrackBoneName.ToString(), FColor::Blue, 10.f);

						CO->SetActorRotation(DeltaRotation);
					}
				}
			}
			else if (LoopType == ELoopType::Track)
			{
				Velocity = CO->GetActorForwardVector() * Speed;
				FVector NexeLocation = StartLocation + (Velocity * GetWorld()->GetDeltaSeconds());
				CO->SetActorLocation(NexeLocation);

				if (TrackChr)
				{

					FRotator CurrentRot = CO->GetActorRotation();
					FVector OriginLoc = CO->GetActorLocation();
					FVector TargetLoc = TrackBoneName == NAME_None ? TrackChr->GetActorLocation() : TrackChr->GetMesh()->GetSocketLocation(TrackBoneName);

					//float CurrentDeltaYaw =  UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TrackChr->GetActorLocation()).Yaw;
					//FRotator NewRot = FRotator(CurrentRot.Pitch, CurrentDeltaYaw, CurrentRot.Roll);

					FRotator DeltaRotation = UKismetMathLibrary::FindLookAtRotation(CO->GetActorLocation(), TargetLoc);
					CurrentRot = UKismetMathLibrary::RInterpTo(CurrentRot, DeltaRotation, GetWorld()->GetDeltaSeconds(), HomingSpeed);
					//CurrentRot = DeltaRotation * InDeltaTime;
					//float CurrentDistance = FMath::Abs((GetActorLocation() - SpawnOwner->GetActorLocation()).Size());
					//float TargetDistance = FMath::Abs((TrackChr->GetActorLocation() - SpawnOwner->GetActorLocation()).Size());
					float TargetDistance = FMath::Abs((TargetLoc - CO->GetActorLocation()).Size());
					if (TargetDistance > 30.0f)
					{
						CO->SetActorRotation(DeltaRotation);
						SoulDebug::ScreenLog(TrackBoneName.ToString(), FColor::Blue, 10.f);

						//if (!bStopMovement) 
					}
					////Multicast_UpdatedProjectile(CurrentRot);
					//CO->AddActorWorldRotation(FQuat(-10.0f, -10.0f, 0.0f, 0.0f));
					//
					//
					////FVector NextLocation = StartLocation + (TargetLoc * InDeltaTime);
					//FVector NextLocation = UKismetMathLibrary::VInterpTo(StartLocation, TargetLoc, InDeltaTime, HomingSpeed);
				}
			}


		}
	}
	else
	{
		Server_UpdatedProjectile(InDeltaTime);
	}

	//Multicast_UpdatedProjectile(InDeltaTime);
}

void UProjectileComponent::Server_UpdatedProjectile_Implementation(float InDeltaTime)
{
	UpdatedProjectile(InDeltaTime);
}

FVector UProjectileComponent::CalculateGravityAndDecelaration(FVector InVelocity, float InDeltaTime)
{
	float DragForce = GetDragForce();								
	float GravityForce = (Gravity * 100.0f * Mass * -1.0f);			
	float DragGravityForce = DragForce + GravityForce;

	FVector DragLocation = FVector(0.0f, 0.0f, DragGravityForce) * InDeltaTime;	

	return InVelocity + DragLocation;
}

float UProjectileComponent::GetDragForce()
{
	float Drag = ((((AirDensity * Speed*Speed) * CrossSection)* DragCoeffciency) / 2.0f) * -1.0f;
	return Drag;
}

float UProjectileComponent::CalculateForwardPitch(FVector NextLocation, FVector CurrentLocation, FRotator CurrenRotation, float InDeltaTime)
{
	FRotator DeltaRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, NextLocation);
	FRotator InnerRotation = UKismetMathLibrary::InverseTransformRotation(FTransform(CurrenRotation, CurrentLocation), DeltaRotation);

	return InnerRotation.Pitch;// * InDeltaTime;
}

void UProjectileComponent::DrawProjectileDebug()
{
	AActor* CO = GetOwner();
	if(!CO) return;

	FHitResult OutHit;
	TArray<FVector> OutPathPositions;
	FVector LastPosition;
	FVector StartPos = CO->GetActorLocation();
	FVector NewVelocity = CO->GetActorForwardVector() * Speed;
	float CurrentTime = 0.0f;

	while (CurrentTime < MaxDrawTime)
	{
		if (LoopType == ELoopType::Projectile)
		{

			FVector NextLocation = StartPos + (NewVelocity * GetWorld()->GetDeltaSeconds());
			//CO->SetActorLocation(NextLocation);
			StartPos = NextLocation;
			NewVelocity = CalculateGravityAndDecelaration(NewVelocity, GetWorld()->GetDeltaSeconds());
			CurrentTime += GetWorld()->GetDeltaSeconds();

			OutPathPositions.Add(NextLocation);
			//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), CO->GetActorLocation(), 5.0f);
		}
	}

	for (int64 i = 0; i < OutPathPositions.Num(); i+=3)
	{
		if (SoulDebug::ShouldDraw(bDrawDebug))
		{
			UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutPathPositions[i], 5.0f, 12, FLinearColor::Green, 50.0f);
		}

		//GEngine->AddOnScreenDebugMessage(-1,10.0f,FColor::White,__FUNCTION__);
	}
	//bool bHit = UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(
	//	GetWorld(), 
	//	OutHit, OutPathPositions, LastPosition, StartPos, Velocity, 
	//	true, 10.f, ECollisionChannel::ECC_WorldStatic, false, {}, EDrawDebugTrace::ForDuration, 15.f, 15.f, 100.0f);

}


