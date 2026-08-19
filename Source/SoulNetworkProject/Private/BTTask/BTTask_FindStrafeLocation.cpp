// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_FindStrafeLocation.h"
#include "Classes/AIControllerBase.h"
#include "Classes/CharacterBase.h"
#include "Classes/AIManager.h"
#include "SoulNetworkProject/Blackboard_Keys.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_FindStrafeLocation::UBTTask_FindStrafeLocation(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Find Strafe Location";

	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);
}

EBTNodeResult::Type UBTTask_FindStrafeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Con = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (Con && LocationSeekerQuery)
	{
		LocationSeekerQueryRequest = FEnvQueryRequest(LocationSeekerQuery, Con->Agent);
		LocationSeekerQueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &UBTTask_FindStrafeLocation::LocationSeekerQueryFinished);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_FindStrafeLocation::LocationSeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	int32 Index = 0;
	float CurrentBestScore =0;
	TArray<FVector> Locations;
	Result->GetAllAsLocations(Locations);

	for (auto& Loc : Locations)
	{
		if (Index == 0)
		{
			StrafeLocation = Loc;
			CurrentBestScore = Result->GetItemScore(Index);
			Index++;
			continue;
		}
		float BeforeBetweenLength = (Con->Agent->GetActorLocation() - StrafeLocation).Size();
		float AfterBetweenLength = (Con->Agent->GetActorLocation() - Loc).Size();
		if (IsApplicableLocation(Loc) && Result->GetItemScore(Index) >= CurrentBestScore && AfterBetweenLength <= BeforeBetweenLength)
		{
			//int8 RandomInt = 1;
			//if(BeforeBetweenLength == AfterBetweenLength) RandomInt = FMath::RandRange(0, 1);
			//if (RandomInt == 0)
			//{
			//}
				StrafeLocation = Loc;
				CurrentBestScore = Result->GetItemScore(Index);

		}

		Index++;
	}

	Con->BBC->SetValueAsVector(bb_keys::target_location, StrafeLocation);
}

bool UBTTask_FindStrafeLocation::IsApplicableLocation(FVector Location)
{
	if(!Con || !Con->Agent) return true;

	bool ConsiderLocation = true;
	TArray<AActor*> OutActors;

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Location, ConditionRadius, ObjectTypes, ACharacterBase::StaticClass(), {}, OutActors);
	if (bHit)
	{
		for (auto& OutActor : OutActors)
		{
			if(Con->Agent == OutActor) return false;
		}
	}

	return ConsiderLocation;
}
