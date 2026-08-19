// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/LockonSystemComponent.h"
#include "Classes/PlayerCharacter.h"
#include "Classes/PlayerControllerBase.h"

//#include "Gameframework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ShapeComponent.h"
#include "Gameframework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"

#include "DrawDebugHelpers.h"

#include "Net/UnrealNetwork.h"
#include "SoulNetworkDebug.h"
#include "Components/BoxComponent.h"


// Sets default values for this component's properties
ULockonSystemComponent::ULockonSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	SetIsReplicated(true);
}


// Called when the game starts
void ULockonSystemComponent::BeginPlay()
{
	Super::BeginPlay();                                       

	// ...
	
}


// Called every frame
void ULockonSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (SoulDebug::ShouldDraw(bShowDebug)) UpdatedSphereDebug(DeltaTime);
	UpdatedLockon(DeltaTime);
	
}

void ULockonSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULockonSystemComponent, bCanSwitch);
	DOREPLIFETIME(ULockonSystemComponent, CurrentBlineTime);

	DOREPLIFETIME(ULockonSystemComponent, LockData);
}



void ULockonSystemComponent::StartLock()
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if(!CO) return;

	if (!LockData.LockChr)
	{
		FLockData CurrentData = GetFindOptimalTarget();
		SetFocusActor(CurrentData.LockChr, CurrentData.LockBoneName);
	}
	else
	{
		SetFocusActor(nullptr);
	}

}

void ULockonSystemComponent::SetFocusActor(AActor* InActor, FName BoneName)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
		if (!CO || !bCanSwitch) return;

		if (InActor)
		{
			if (ACharacterBase* Chr = Cast<ACharacterBase>(InActor))
			{
				if (CO->GetFaction() != Chr->GetFaction() && !Chr->GetDead())
				{
					if (LockData.LockChr) BindWidget(LockData.LockChr, LockData.LockBoneName, false);

					LockData.LockChr = Chr;
					LockData.LockBoneName = BoneName;
					BindWidget(LockData.LockChr, LockData.LockBoneName, true);



				}
				else
				{
					if (LockData.LockChr) BindWidget(LockData.LockChr, LockData.LockBoneName,  false);

					LockData.LockChr = nullptr;
					LockData.LockBoneName = NAME_None;
					CurrentBlineTime = 0.0f;
					CO->Server_SetUseControllerYaw(false);
				}
			}
			else
			{
				if (LockData.LockChr) BindWidget(LockData.LockChr, LockData.LockBoneName, false);

				LockData.LockChr = nullptr;
				LockData.LockBoneName = NAME_None;
				CurrentBlineTime = 0.0f;
				CO->Server_SetUseControllerYaw(false);
			}
		}
		else
		{
			if (LockData.LockChr) BindWidget(LockData.LockChr, LockData.LockBoneName, false);

			LockData.LockChr = nullptr;
			LockData.LockBoneName = NAME_None;
			CurrentBlineTime = 0.0f;
			CO->Server_SetUseControllerYaw(false);
		}

		bCanSwitch = false;
		GetWorld()->GetTimerManager().SetTimer(SwitchHandle, FTimerDelegate::CreateWeakLambda(this, [&]
		{
			bCanSwitch = true;
		}), TimerDelay, false);
	}
	else
	{
		Server_SetFocusActor(InActor, BoneName);
	}
}

void ULockonSystemComponent::Server_SetFocusActor_Implementation(AActor* InActor, FName BoneName)
{
	SetFocusActor(InActor, BoneName);
}

void ULockonSystemComponent::BindWidget_Implementation(AActor* BindActor, FName BindBoneName, bool bVisibility)
{
	ACharacterBase* BindChr = Cast<ACharacterBase>(BindActor);
	if(!BindChr) return;

	TArray<USceneComponent*> GetAttachComponents;

	BindChr->HealthWidget->SetHiddenInGame(!bVisibility);
	BindChr->GetMesh()->GetChildrenComponents(true, GetAttachComponents);
	for (USceneComponent* GetAttachComponent : GetAttachComponents)
	{
		if (UWidgetComponent* WC = Cast<UWidgetComponent>(GetAttachComponent))
		{
			
			if (BindBoneName == WC->GetAttachSocketName())
			{
				WC->SetHiddenInGame(!bVisibility);
			}
		}
	}
}

void ULockonSystemComponent::Client_UpdatedConRot_Implementation(FRotator InRot)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if(!CO) return;

	if (!CO->GetHasCrowdControl())
	{
		CO->GetController()->SetControlRotation(InRot);
	}
}

void ULockonSystemComponent::UpdatedLockon(float InDeltaTime)
{
	//if (!Owner) return;
	if (GetOwnerRole() == ROLE_Authority)
	{
		APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
		if (CO && LockData.LockChr)
		{
			FVector StartLoc = CO->GetActorLocation();
			FVector TargetLoc = LockData.LockChr->GetMesh()->GetSocketLocation(LockData.LockBoneName);
			FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(StartLoc, TargetLoc);
			
			FRotator ConRot = CO->GetController()->GetControlRotation();
			FRotator InterpRot = UKismetMathLibrary::RInterpTo(ConRot, TargetRot, InDeltaTime, InterpSpeed);
			FRotator NewRot = FRotator(InterpRot.Pitch, InterpRot.Yaw, ConRot.Roll);

			if (!CO->GetHasCrowdControl())
			{
				Client_UpdatedConRot(InterpRot); //NewRot
				CO->AnimValues.bRoll ?
					CO->Server_SetUseControllerYaw(false) :
					CO->Server_SetUseControllerYaw(CO->GetCharacterMovement()->MaxWalkSpeed > CO->GetWalkSpeed() + 1.f ? false : true);
			}

			FVector Start = CO->GetActorLocation();
			FVector End = LockData.LockChr->GetActorLocation();
			FHitResult OutHit;

			bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 10.0f, TraceTypeQuery1, false, {GetOwner()}, EDrawDebugTrace::None, OutHit, false);
			if (bHit)
			{
				if (ACharacterBase* Chr = Cast<ACharacterBase>(OutHit.GetActor()))
				{
					if (CO->IsHostile(Chr)) CurrentBlineTime = 0.f;
				}
				else
				{
					CurrentBlineTime += InDeltaTime;
					if (CurrentBlineTime >= MaxBlindTime)
					{
						SetFocusActor(nullptr);
						return;
					}
				}
			}

		}
	}
	else
	{
		Server_UpdatedLockon(InDeltaTime);
	}
}
void ULockonSystemComponent::Server_UpdatedLockon_Implementation(float InDeltaTime)
{
	UpdatedLockon(InDeltaTime);
}

void ULockonSystemComponent::UpdatedSphereDebug_Implementation(float InDeltaTime)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if (!CO) return;

	TArray<UPrimitiveComponent*> OutComponents;
	bool bHit = UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), CO->GetActorLocation(), MaxSphereRadius, { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel5) }, UShapeComponent::StaticClass(), { GetOwner() }, OutComponents);
	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), CO->GetActorLocation(), MaxSphereRadius, 12, FLinearColor::White);
	if (bHit)
	{
		for (UPrimitiveComponent* PC : OutComponents)
		{
			if (UShapeComponent* SC = Cast<UShapeComponent>(PC))
			{
				if (!SC->GetOwner()) continue;
				else if (ACharacterBase* Chr = Cast<ACharacterBase>(SC->GetOwner()))
				{
					//if(LockData.LockChr && LockData.LockChr == Chr) continue;

					//if (CO->GetFaction() == Chr->GetFaction()) UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Chr->GetMesh()->GetSocketLocation(SC->GetAttachSocketName()), 50.0f, 12, FLinearColor::Green, 2.0f);
					//else UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Chr->GetMesh()->GetSocketLocation(SC->GetAttachSocketName()), 50.0f, 12, FLinearColor::Red, 2.0f);
					
					FVector2D ScreenLocation = FVector2D::ZeroVector;
					bool bResult = CO->GetPC()->ProjectWorldLocationToScreen(Chr->GetMesh()->GetSocketLocation(SC->GetAttachSocketName()), ScreenLocation, false);
					const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
					float XDistance = FMath::Abs(ScreenLocation.X - ViewportSize.X / 2.0f);//.Size();
					float YLocation = ScreenLocation.Y;

					//TTuple<float, bool> ResultX = GetScreenYDistanceOfActor(Chr, SC->GetAttachSocketName());
					float TargetYawFromCO = GetLocalYawToTargetFromLockChr(Chr, SC->GetAttachSocketName());

					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, Chr->GetName());
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Distance: %f"), ResultX.Get<0>()));
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("ResultLocation X: %f"), FMath::Abs(ResultLocation.X)));// - ViewportSize.X / 2.0f));
					//GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Green, FString::Printf(TEXT("ResultLocation Y: %f"), FMath::Abs(ResultLocation.Y)));
					//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("XDistance for abs: %f"), FMath::Abs(XDistance)));
					//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("YLocation: %f"), YLocation));
					DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 100.0f), FString::Printf(TEXT("TargetYawFromCO: %1f"), TargetYawFromCO), Chr, FColor::White, InDeltaTime);
				}
			}
		}
	}
}

void ULockonSystemComponent::CheckDirectionScreenPoint(float AxisValue, bool bForward)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if (!CO || !bCanSwitch) return;

	bForward ? InputY = AxisValue : InputX = AxisValue;

	if (FMath::Abs(AxisValue) >= 5.0f)
	{
		if (LockData.LockChr)
		{
			TTuple<float, bool> LockChrXInfo = GetScreenXDistanceOfActor(LockData.LockChr, LockData.LockBoneName);
			TTuple<float, bool> LockChrYInfo = GetScreenYDistanceOfActor(LockData.LockChr, LockData.LockBoneName);

			TArray<FLockData> SearchDatas = GetSearchAgents();
			TArray<FLockData> PickDatas;
	
			for (FLockData& SearchData : SearchDatas)
			{
				if (LockData.LockChr == SearchData.LockChr && LockData.LockBoneName == SearchData.LockBoneName) continue;
				if (!CanSearchTargetInArea(SearchData.LockChr, SearchData.LockBoneName)) continue;

				TTuple<float, bool> ResultXInfo = GetScreenXDistanceOfActor(SearchData.LockChr, SearchData.LockBoneName);
				TTuple<float, bool> ResultYInfo = GetScreenYDistanceOfActor(SearchData.LockChr, SearchData.LockBoneName);
				if (!ResultXInfo.Get<1>() || !ResultYInfo.Get<1>() || !HasActorInViewport(SearchData.LockChr, SearchData.LockBoneName)) continue;

				//InputX//
				if (FMath::Abs(InputX) > 3.0f && FMath::Abs(InputY) < 3.0f)
				{
					if (InputX < -1.0f && ResultXInfo.Get<0>() < LockChrXInfo.Get<0>()) PickDatas.Add(SearchData);
					else if (InputX > 1.0f && ResultXInfo.Get<0>() > LockChrXInfo.Get<0>()) PickDatas.Add(SearchData);
				}
				//InputY//
				else if (FMath::Abs(InputX) < 3.0f && FMath::Abs(InputY) > 3.0f)
				{
					if (InputY > 1.0f && ResultYInfo.Get<0>() > LockChrYInfo.Get<0>()) PickDatas.Add(SearchData);
					else if (InputY < -1.0f && ResultYInfo.Get<0>() < LockChrYInfo.Get<0>()) PickDatas.Add(SearchData);
				}
				//InputX, InputY//
				else if (FMath::Abs(InputX) > 3.0f && FMath::Abs(InputY) > 3.0f)
				{
					if (InputY > 1.0f && ResultYInfo.Get<0>() > LockChrYInfo.Get<0>())
					{
						if(InputX < -1.0f && ResultXInfo.Get<0>() < LockChrXInfo.Get<0>()) PickDatas.Add(SearchData);
						else if(InputX > 1.0f && ResultXInfo.Get<0>() > LockChrXInfo.Get<0>()) PickDatas.Add(SearchData);
					}
					else if (InputY < -1.0f && ResultYInfo.Get<0>() < LockChrYInfo.Get<0>())
					{
						if (InputX < -1.0f && ResultXInfo.Get<0>() < LockChrXInfo.Get<0>()) PickDatas.Add(SearchData);
						else if (InputX > 1.0f && ResultXInfo.Get<0>() > LockChrXInfo.Get<0>()) PickDatas.Add(SearchData);
					}
				}
			}
			if(PickDatas.Num() == 0)  CheckDirectionYaw(AxisValue, bForward);
			else
			{
				FLockData BestData;

				float MainDistance = 0.0f;
				float SecondaryDistance = 0.0f;
				for (FLockData& PickData : PickDatas)
				{
					if(!BestData.LockChr) BestData = PickData;
					else
					{
						float ScreenRatio = GetScreenSizeRatio();
						float PickTotalScreenPoint =
							FMath::Abs(GetScreenXDistanceOfActor(PickData.LockChr, PickData.LockBoneName).Get<0>()) +
							(FMath::Abs(GetScreenYDistanceOfActor(PickData.LockChr, PickData.LockBoneName).Get<0>()) * ScreenRatio);

						float BestTotalScreenPoint =
							FMath::Abs(GetScreenXDistanceOfActor(BestData.LockChr, BestData.LockBoneName).Get<0>()) +
							(FMath::Abs(GetScreenYDistanceOfActor(BestData.LockChr, BestData.LockBoneName).Get<0>()) * ScreenRatio);

						if(PickTotalScreenPoint < BestTotalScreenPoint) BestData = PickData;
					}
				
				}
				if(BestData.LockChr) SetFocusActor(BestData.LockChr, BestData.LockBoneName);
				else CheckDirectionYaw(AxisValue, bForward);
			}
		}
	}
}

void ULockonSystemComponent::CheckDirectionYaw(float AxisValue, bool bForward)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if (!CO || !bCanSwitch) return;

	bForward ? InputY = AxisValue : InputX = AxisValue;

	if (FMath::Abs(AxisValue) >= 5.0f)
	{
		if (LockData.LockChr)
		{
			TArray<FLockData> SearchDatas = GetSearchAgents();
			TArray<FLockData> PickDatas;

			for (FLockData& SearchData : SearchDatas)
			{
				if(LockData.LockChr == SearchData.LockChr && LockData.LockBoneName == SearchData.LockBoneName) continue;
				if (!CanSearchTargetInArea(SearchData.LockChr, SearchData.LockBoneName)) continue;

				float TargetYawFromLockChr = GetLocalYawToTargetFromLockChr(SearchData.LockChr, SearchData.LockBoneName);
				
				if (FMath::Abs(InputX) > 3.0f && FMath::Abs(InputY) < 3.0f)
				{
					if(InputX > 0.0f && (TargetYawFromLockChr >= 45.0f || TargetYawFromLockChr <= 135.0f)) PickDatas.Add(SearchData);
					else if(InputX < 0.0f && (TargetYawFromLockChr <= -45.0f || TargetYawFromLockChr >= -135.0f))PickDatas.Add(SearchData);
				}
				else if (FMath::Abs(InputY) > 3.0f && FMath::Abs(InputX) < 3.0f)
				{
					if (InputY < 0.0f && (TargetYawFromLockChr >= -45.0f || TargetYawFromLockChr <= 45.0f)) PickDatas.Add(SearchData);
					else if (InputY > 0.0f && (TargetYawFromLockChr <= -135.0f || TargetYawFromLockChr >= 135.0f)) PickDatas.Add(SearchData);
				}
				else if(FMath::Abs(InputX) > 3.0f && FMath::Abs(InputY) > 3.0f)
				{
					if (InputY < 0.0f)
					{
						if (InputX < 0.0f && (TargetYawFromLockChr >= 0.0f && TargetYawFromLockChr <= 90.0f)) PickDatas.Add(SearchData);
						else if (InputX > 0.0f && (TargetYawFromLockChr <= 0.0f && TargetYawFromLockChr >= -45.0f))  PickDatas.Add(SearchData);
					}
					else if (InputY > 0.0f)
					{
						if (InputX < 0.0f && (TargetYawFromLockChr >= 90.0f && TargetYawFromLockChr <= 180.0f)) PickDatas.Add(SearchData);
						else if (InputX > 0.0f && (TargetYawFromLockChr <= -90.0f && TargetYawFromLockChr >= -180.0f)) PickDatas.Add(SearchData);
					}
				}

				if(PickDatas.Num() == 0) return;
				else
				{
					FLockData BestData;
					for (FLockData& PickData : PickDatas)
					{
						if(!BestData.LockChr) BestData = PickData;
						else
						{
							float BestDistance = GetDepthDistanceToTaget(BestData.LockChr, BestData.LockBoneName);
							float PickDistance = GetDepthDistanceToTaget(PickData.LockChr, PickData.LockBoneName);

							if(PickDistance < BestDistance) BestData = PickData;
						}
					}
					SetFocusActor(BestData.LockChr, BestData.LockBoneName);
				}
			}
		}
	}
}



TArray<FLockData> ULockonSystemComponent::GetSearchAgents()
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if (!CO) return {};

	TArray<FLockData> OutDatas;
	TArray<UPrimitiveComponent*> OutComponents;

	bool bHit = UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), CO->GetActorLocation(), MaxSphereRadius, { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel5) }, UShapeComponent::StaticClass(), { GetOwner() }, OutComponents);
	if (SoulDebug::ShouldDraw(bShowDebug))
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), CO->GetActorLocation(), MaxSphereRadius, 12, FLinearColor::Green);
	}
	if (bHit)
	{
		for (UPrimitiveComponent* OutComponent : OutComponents)
		{
			if (UBoxComponent* BC = Cast<UBoxComponent>(OutComponent))
			{
				if (SoulDebug::ShouldDraw(bShowDebug))
				{
					UKismetSystemLibrary::DrawDebugSphere(GetWorld(), BC->GetComponentLocation(), 50.0f, 12, FLinearColor::Red);
				}
				if (ACharacterBase* Chr = Cast<ACharacterBase>(BC->GetOwner()))
				{
					if (CO->GetFaction() == Chr->GetFaction()) continue;

					FLockData NewData;
					NewData.LockChr = Chr;
					NewData.LockBoneName = BC->GetAttachSocketName();

					OutDatas.Add(NewData);
				}
			}
		}
	}
	return OutDatas;
}

FLockData ULockonSystemComponent::GetFindOptimalTarget()
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if (!CO) return {};

	//TArray<AActor*> OutActors;
	float CurrentDistance = 0.0f;
	FLockData BestData;
	
	TArray<FLockData> SearchDatas = GetSearchAgents();
	for (FLockData& SearchData : SearchDatas)
	{
		TTuple<float, bool> ResultXInfo = GetScreenXDistanceOfActor(SearchData.LockChr, SearchData.LockBoneName);
		TTuple<float, bool> ResultYInfo = GetScreenYDistanceOfActor(SearchData.LockChr, SearchData.LockBoneName);
		if (!ResultXInfo.Get<1>() || !ResultYInfo.Get<1>() || !HasActorInViewport(SearchData.LockChr, SearchData.LockBoneName)) continue;
		if (!CanSearchTargetInArea(SearchData.LockChr, SearchData.LockBoneName)) continue;

		float ResultDistance = FMath::Abs(ResultXInfo.Get<0>()) + (FMath::Abs(ResultYInfo.Get<0>()) * GetScreenSizeRatio());

		if (!BestData.LockChr)
		{
			BestData = SearchData;
			CurrentDistance = ResultDistance;
		}
		else
		{
			if (ResultDistance <= CurrentDistance)
			{
				BestData = SearchData;
				CurrentDistance = ResultDistance;

				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Passed Valid Chr and lowely distance"));
			}
		}

	}

	return BestData;
}

float ULockonSystemComponent::CalculateDotProductToTarget(ACharacterBase* Target, FName TargetSocketName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if(!CO || !Target) return 0.0f;


	// DynamicLockOn
	if (LockData.LockChr)
	{
		
		const FVector NewTargetableActorDirection = UKismetMathLibrary::GetDirectionUnitVector(
			//LockData.LockChr->GetMesh()->GetSocketLocation(LockData.LockBoneName) + 
			CO->GetCamera()->GetComponentLocation(),
			Target->GetMesh()->GetSocketLocation(TargetSocketName));
		
		const FVector CrossProduct = UKismetMathLibrary::Cross_VectorVector(CO->GetCamera()->GetForwardVector(), NewTargetableActorDirection);

		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green, NewTargetableActor->GetName());
		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Red,FString::Printf(TEXT("NeTarget Direction: %f, %f, %f"), CrossProduct.X, CrossProduct.Y, CrossProduct.Z));
		return UKismetMathLibrary::Dot_VectorVector(CO->GetCamera()->GetUpVector(), CrossProduct);
	}

	return 0.0f;
}

float ULockonSystemComponent::CalcualteDotProductToTargetY(ACharacterBase* Target, FName TargetSocketName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if (!CO || !Target) return 0.0f;


	// DynamicLockOn
	if (LockData.LockChr)
	{
		//if (PlayerCameraManager)
		{
			const FVector NewTargetableActorDirection = UKismetMathLibrary::GetDirectionUnitVector(CO->GetCamera()->GetComponentLocation(),
				Target->GetMesh()->GetSocketLocation(TargetSocketName));
			const FVector CrossProduct = UKismetMathLibrary::Cross_VectorVector(CO->GetCamera()->GetForwardVector(), NewTargetableActorDirection);
			return UKismetMathLibrary::Dot_VectorVector(CO->GetCamera()->GetUpVector(), CrossProduct);

		}
	}

	return 0.0f;
}

float ULockonSystemComponent::GetLocalYawToTargetFromLockChr(class ACharacterBase* Target, FName TargetSocketName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if (!CO || !Target) return 0.0f;

	if (LockData.LockChr)
	{
		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(
			LockData.LockChr->GetMesh()->GetSocketLocation(LockData.LockBoneName) + CO->GetCamera()->GetForwardVector(), 
			Target->GetMesh()->GetSocketLocation(TargetSocketName));
		FRotator InnerRotation = UKismetMathLibrary::InverseTransformRotation(CO->GetActorTransform(), LookRotation);

		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green, Target->GetName());
		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Red,FString::Printf(TEXT("NormalForwardVector: %f, %f, %f"), TargetLocation.X, TargetLocation.Y, TargetLocation.Z));


		//TargetDistance

		return InnerRotation.Yaw;
	}

	return 0.0f;
}

float ULockonSystemComponent::GetLocalYawToTargetFromCO(ACharacterBase* Target, FName TagetSocketName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if (!CO || !Target) return 0.0f;

	if (LockData.LockChr)
	{
		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(
			CO->GetActorLocation(), 
			Target->GetMesh()->GetSocketLocation(TagetSocketName));
		FRotator InnerRotation = UKismetMathLibrary::InverseTransformRotation(CO->GetActorTransform(), LookRotation);

		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green, Target->GetName());
		//GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Red,FString::Printf(TEXT("NormalForwardVector: %f, %f, %f"), TargetLocation.X, TargetLocation.Y, TargetLocation.Z));


		//TargetDistance

		return InnerRotation.Yaw;
	}

	return 0.0f;
}

float ULockonSystemComponent::GetDepthDistanceToTaget(ACharacterBase* Target, FName TargetSocketName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if (!CO || !Target) return 0.0f;

	if (LockData.LockChr)
	{
		float DepthDistance = 
			(LockData.LockChr->GetMesh()->GetSocketLocation(LockData.LockBoneName) - 
						Target->GetMesh()->GetSocketLocation(TargetSocketName)).Size();

		return DepthDistance;
	}

	return 0.0f;
}


float ULockonSystemComponent::GetScreenPositionOfActor(AActor* SearchActor, FName SearchBoneName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	ACharacterBase* Chr = Cast<ACharacterBase>(GetOwner());
	if(!CO || !Chr) return 0.0f;


	FVector2D ScreenLocation = FVector2D::ZeroVector;
	bool bResult = CO->GetPC()->ProjectWorldLocationToScreen(Chr->GetActorLocation(), ScreenLocation, false);
	
	const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
	if(ScreenLocation.X < 0.0f  || ScreenLocation.X > ViewportSize.X || ScreenLocation.Y < 0.0f || ScreenLocation.Y > ViewportSize.Y) return 0.0f;

	FVector2D ResultLocation = ScreenLocation - ViewportSize / 2.0f;
	return FMath::Abs(ScreenLocation.X - ViewportSize.X / 2.0f);
}

bool ULockonSystemComponent::CanSearchTargetInArea(ACharacterBase* Target, FName TargetSocketName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	if(!CO || !Target) return false;
	
	float CoToTargetDistance = (CO->GetActorLocation() - Target->GetMesh()->GetSocketLocation(TargetSocketName)).Size();
	if (LockData.LockChr)
	{
		float LockChrToTargetDistance = (LockData.LockChr->GetMesh()->GetSocketLocation(LockData.LockBoneName) - Target->GetMesh()->GetSocketLocation(TargetSocketName)).Size();
		if(CoToTargetDistance > MaxSearchDistanceCoToTarget || LockChrToTargetDistance > MaxSearchDistanceLockChrToTarget) return false;
	}

	return CoToTargetDistance < MaxSearchDistanceCoToTarget ? true : false;
}
TTuple<float, bool> ULockonSystemComponent::GetScreenXDistanceOfActor(AActor* SearchActor, FName SearchBoneName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	ACharacterBase* Chr = Cast<ACharacterBase>(SearchActor);
	if(!CO || !Chr) return MakeTuple(0.0f, false);

	FVector2D ScreenLocation = FVector2D::ZeroVector;
	bool bResult = CO->GetPC()->ProjectWorldLocationToScreen(Chr->GetMesh()->GetSocketLocation(SearchBoneName), ScreenLocation, true);
	//bool bResult = UGameplayStatics::ProjectWorldToScreen(Cast<APlayerControllerBase>(CO->GetController()), Chr->GetMesh()->GetSocketLocation(SearchBoneName), ScreenLocation, true);

	const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
	float ResultXDistance = ScreenLocation.X - ViewportSize.X / 2.0f;

	if(ScreenLocation.X < 0.0f || ScreenLocation.X > ViewportSize.X) return MakeTuple(ResultXDistance, false);

	
	return MakeTuple(ResultXDistance, bResult);
}
TTuple<float, bool> ULockonSystemComponent::GetScreenYDistanceOfActor(AActor* SearchActor, FName SearchBoneName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	ACharacterBase* Chr = Cast<ACharacterBase>(SearchActor);
	if (!CO || !Chr) return MakeTuple(0.0f, false);

	FVector2D ScreenLocation = FVector2D::ZeroVector;
	bool bResult = CO->GetPC()->ProjectWorldLocationToScreen(Chr->GetMesh()->GetSocketLocation(SearchBoneName), ScreenLocation, true);
	//bool bResult = UGameplayStatics::ProjectWorldToScreen(Cast<APlayerControllerBase>(CO->GetController()), Chr->GetMesh()->GetSocketLocation(SearchBoneName), ScreenLocation, true);

	const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
	float ResultYDistance = ScreenLocation.Y - ViewportSize.Y / 2.0f;

	if (ScreenLocation.Y < 0.0f || ScreenLocation.Y > ViewportSize.Y) return MakeTuple(ResultYDistance, false);


	return MakeTuple(ResultYDistance, bResult);
}

float ULockonSystemComponent::GetScreenSizeRatio()
{
	const FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
	float ScreenRatio = ViewportSize.X / ViewportSize.Y;


	return ScreenRatio;
}

bool ULockonSystemComponent::HasActorInViewport(AActor* SearchActor, FName SearchBoneName)
{
	APlayerCharacter* CO = Cast<APlayerCharacter>(GetOwner());
	ACharacterBase* Chr = Cast<ACharacterBase>(SearchActor);
	if(!CO || !Chr) return false;

	FHitResult OutHit;
	FVector Start = CO->GetCamera()->GetComponentLocation();
	FVector End = Chr->GetMesh()->GetSocketLocation(SearchBoneName);
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility);
	if (bHit)
	{
		if (ACharacterBase* HitChr = Cast<ACharacterBase>(OutHit.GetActor()))
		{
			if(HitChr == Chr) return true;
		}
	}

	return false;
}
