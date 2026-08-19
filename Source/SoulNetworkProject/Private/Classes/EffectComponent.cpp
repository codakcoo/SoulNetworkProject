// Fill out your copyright notice in the Description page of Project Settings.


#include "Classes/EffectComponent.h"
#include "Classes/CharacterBase.h"

#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#define TRACE_START TEXT("Trace_Start")
#define TRACE_END TEXT("Trace_End")

// Sets default values for this component's properties
UEffectComponent::UEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	static ConstructorHelpers::FObjectFinder<UDataTable> EffectDt_Obj(TEXT("DataTable'/Game/BP/Character/DataTable/EffectData.EffectData'"));
	if (EffectDt_Obj.Succeeded())
	{
		EffectDt = EffectDt_Obj.Object;
	}
}


// Called when the game starts
void UEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UEffectComponent::EffectSequence(EEffectType EffectType, FVector EffectLocation, FHitResult HitResult)
{
	FEffectData* EffectData = nullptr;

	switch (EffectType)
	{
	case EEffectType::Damaged:
	{
		EffectData = GetEffectData("Hit");
		Server_ReactionEffect(EffectData->Sound, EffectData->Niagara, EffectLocation);
	}
	break;
	case EEffectType::Block:
	{
		EffectData = GetEffectData("Block");
		Server_ReactionEffect(EffectData->Sound, EffectData->Niagara, EffectLocation);
	}
	break;
	case EEffectType::Health:
	{
		EffectData = GetEffectData("Health");
		Server_ReactionEffect(EffectData->Sound, EffectData->Niagara, EffectLocation);
	}
	break;
	case EEffectType::Mana:
	{
		EffectData = GetEffectData("Mana");
		Server_ReactionEffect(EffectData->Sound, EffectData->Niagara, EffectLocation);
	}
	break;
	case EEffectType::Status:
	{
		EffectData = GetEffectData("Status");
		Server_ReactionEffect(EffectData->Sound, EffectData->Niagara, EffectLocation);
	}
	break;
	case EEffectType::Surface:
	{
		FRotator NewRot = UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal) + FRotator(-90.0f, 0.0f, 0.0f);

		EffectData = GetSurfaceEffectData(HitResult.PhysMaterial, EffectDt);
		if (EffectData)	Server_ReactionEffect(EffectData->Sound, EffectData->Niagara, EffectLocation, NewRot);
	}
	break;

	default:
		break;
	}
}

void UEffectComponent::Server_ReactionEffect_Implementation(class USoundBase* PlayCue, class UNiagaraSystem* NiagaraAsset, FVector EffectLocation, FRotator EffectRotation, float EffectScale)
{

	Multicast_ReactionEffect(PlayCue, NiagaraAsset, EffectLocation, EffectRotation, EffectScale);
}
bool UEffectComponent::Server_ReactionEffect_Validate(class USoundBase* PlayCue, class UNiagaraSystem* NiagaraAsset, FVector EffectLocation, FRotator EffectRotation, float EffectScale)
{
	return true;
}

void UEffectComponent::Multicast_ReactionEffect_Implementation(class USoundBase* PlayCue, class UNiagaraSystem* NiagaraAsset, FVector EffectLocation, FRotator EffectRotation, float EffectScale)
{
	PlayEffect(PlayCue, NiagaraAsset, 1.0f, EffectLocation, EffectRotation, EffectScale);
}
bool UEffectComponent::Multicast_ReactionEffect_Validate(class USoundBase* PlayCue, class UNiagaraSystem* NiagaraAsset, FVector EffectLocation, FRotator EffectRotation, float EffectScale)
{
	return true;
}

void UEffectComponent::RunBodyMeshNiagara(FName BuffName)
{
	if (ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner()))
	{
		FEffectData* Data = nullptr;
		Data = GetEffectData(BuffName);

		if (Data)
		{
			TArray<UActorComponent*> BodyComps = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "BODY");
			for (const auto& Mesh : BodyComps)
			{
				if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(Mesh))
				{

					UFXSystemComponent* FXComponent = SpawnNiagara(SkeletalMesh, Data, "root", BuffName);

					if (UNiagaraComponent* NSComponent = Cast<UNiagaraComponent>(FXComponent))
					{
						NSComponent->SetVariableObject("SkeletaltoAttach", SkeletalMesh);

						//GetWorld()->GetTimerManager().SetTimer(NiagaraTimer, FTimerDelegate::CreateLambda([&]
						//	{
						//		ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner());
						//		if (!Owner) return;
						//
						//		TArray<UActorComponent*> BodyComps = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), TEXT("BODY"));
						//		for (const auto& Mesh : BodyComps)
						//		{
						//
						//			if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(Mesh))
						//			{
						//				//Server_RemoveBodyMesh(SkeletalMesh, "BODY");
						//				this->RemoveNiagara(SkeletalMesh, TEXT("BODY"));
						//			}
						//		}
						//	}), Delay, false);
					}
				}
			}
		}
	}
}

void UEffectComponent::RemoveBodyMeshNiagara(FName TagName)
{
	ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner());
	if (!Owner) return;

	TArray<UActorComponent*> BodyComps = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "BODY");
	for (const auto& Mesh : BodyComps)
	{
		if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(Mesh))
		{
			RemoveNiagara(SkeletalMesh, TagName);
		}
	}
}

void UEffectComponent::Multicast_BodyMeshNigara_Implementation(EBuffElementType BuffType, float Delay)
{
	if (BuffType == EBuffElementType::Fire)
	{
		RunBodyMeshNiagara(TEXT("Fire"));
	}
	else if (BuffType == EBuffElementType::Electric)
	{
		RunBodyMeshNiagara(TEXT("Electic"));
	}
}
bool UEffectComponent::Multicast_BodyMeshNigara_Validate(EBuffElementType BuffType, float Delay)
{
	return true;
}

void UEffectComponent::Server_RemoveBodyMesh_Implementation(USkeletalMeshComponent* Mesh, FName TagName)
{
	Multicast_RemoveBodyMesh(Mesh, TagName);
}
bool UEffectComponent::Server_RemoveBodyMesh_Validate(USkeletalMeshComponent* Mesh, FName TagName)
{
	return true;
}


void UEffectComponent::Multicast_RemoveBodyMesh_Implementation(USkeletalMeshComponent* Mesh, FName TagName)
{
	RemoveNiagara(Mesh, TagName);
}
bool UEffectComponent::Multicast_RemoveBodyMesh_Validate(USkeletalMeshComponent* Mesh, FName TagName)
{
	return true;
}

void UEffectComponent::RunWeaopnMeshNiagara(FName BuffName)
{
	ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner());
	if(!Owner) return;

	FEffectData* Data = nullptr;

	Data = GetEffectData(BuffName);

	if (Data)
	{
		if (UFXSystemComponent* FXComponent = SpawnNiagara(Owner->GetMainWeaponMesh(), Data, "Enchant", "ENCHANT"))
		{
			if (UNiagaraComponent* NSComponent = Cast<UNiagaraComponent>(FXComponent))
			{
				NSComponent->SetVariableObject("SkeletaltoAttach", Owner->GetMainWeaponMesh());
			}
		}
	}
}

void UEffectComponent::RemoveWeaponMeshNigara()
{
	ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner());
	if(!Owner) return;

	RemoveNiagara(Owner->GetMainWeaponMesh(), TEXT("ENCHANT"));
}

void UEffectComponent::WeaponTrail(EEquipHand HandType, bool bActive)
{
	if (ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner()))
	{
		if (HandType == EEquipHand::Left)
		{
			if (bActive)
			{
				FVector AttachLocation = (
					Owner->GetSecondaryWeaponMesh()->GetSocketLocation(TRACE_START) + Owner->GetSecondaryWeaponMesh()->GetSocketLocation(TRACE_END)) / 2.f;
				FRotator AttachRotation = UKismetMathLibrary::MakeRotFromZ(
					Owner->GetSecondaryWeaponMesh()->GetSocketLocation(TRACE_START) - Owner->GetSecondaryWeaponMesh()->GetSocketLocation(TRACE_END));

				UFXSystemComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAttached(
					WeaponTraceNigara_Obj,
					Owner->GetSecondaryWeaponMesh(),
					TEXT("Trail_Socket"),
					AttachLocation,
					AttachRotation,
					EAttachLocation::KeepWorldPosition, false);

				if (UNiagaraComponent* NSComponent = Cast<UNiagaraComponent>(Component))
				{
					NSComponent->SetFloatParameter("BeamWidth", (Owner->GetSecondaryWeaponMesh()->GetSocketLocation(TRACE_START) - Owner->GetSecondaryWeaponMesh()->GetSocketLocation(TRACE_END)).Size());
					Component->ComponentTags.AddUnique(TEXT("TRAIL"));
				}
			}
			else
			{
				if (Owner->GetSecondaryWeaponMesh())
				{
					TArray<USceneComponent*> Children;
					Owner->GetSecondaryWeaponMesh()->GetChildrenComponents(false, Children);

					if (Children.Num())
					{
						for (USceneComponent* Component : Children)
						{
							if (Component && Component->ComponentHasTag(TEXT("TRAIL")))
							{
								if (UFXSystemComponent* FXComponent = CastChecked<UFXSystemComponent>(Component))
								{
									FXComponent->ComponentTags.Remove(TEXT("TRAIL"));
									FXComponent->Deactivate();
								}
							}
						}
					}
				}
			}
		}
		else if (HandType == EEquipHand::Right)
		{
			if (bActive)
			{
				FVector AttachLocation = (
					Owner->GetMainWeaponMesh()->GetSocketLocation(TRACE_START) + Owner->GetMainWeaponMesh()->GetSocketLocation(TRACE_END)) / 2.f;
				FRotator AttachRotation = UKismetMathLibrary::MakeRotFromZ(
					Owner->GetMainWeaponMesh()->GetSocketLocation(TRACE_START) - Owner->GetMainWeaponMesh()->GetSocketLocation(TRACE_END));

				UFXSystemComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAttached(
					WeaponTraceNigara_Obj,
					Owner->GetMainWeaponMesh(),
					TEXT("Trail_Socket"),
					AttachLocation,
					AttachRotation,
					EAttachLocation::KeepWorldPosition, false);

				if (UNiagaraComponent* NSComponent = Cast<UNiagaraComponent>(Component))
				{
					NSComponent->SetFloatParameter("BeamWidth", (Owner->GetMainWeaponMesh()->GetSocketLocation(TRACE_START) - Owner->GetMainWeaponMesh()->GetSocketLocation(TRACE_END)).Size());
					Component->ComponentTags.AddUnique(TEXT("TRAIL"));
				}
			}
			else
			{
				if (Owner->GetMainWeaponMesh())
				{
					TArray<USceneComponent*> Children;
					Owner->GetMainWeaponMesh()->GetChildrenComponents(false, Children);

					if (Children.Num())
					{
						for (USceneComponent* Component : Children)
						{
							if (Component && Component->ComponentHasTag(TEXT("TRAIL")))
							{
								if (UFXSystemComponent* FXComponent = CastChecked<UFXSystemComponent>(Component))
								{
									FXComponent->ComponentTags.Remove(TEXT("TRAIL"));
									FXComponent->Deactivate();
								}
							}
						}
					}
				}
			}
		}
	}
}

void UEffectComponent::AllMeshFadeout(float FadeValue)
{
	if (ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner()))
	{
		TArray<UActorComponent*> BodyComps = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "BODY");
		for (const auto& Mesh : BodyComps)
		{
			if (USkeletalMeshComponent* BodyMesh = Cast<USkeletalMeshComponent>(Mesh))
			{
				const TCHAR* Path = *BODY_PATH;
				FadeoutMesh(BodyMesh, Path, FadeValue);
			}
		}

		TArray<UActorComponent*> ClothComps = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "CLOTH");
		for (const auto& Mesh : ClothComps)
		{
			if (USkeletalMeshComponent* ClothMesh = Cast<USkeletalMeshComponent>(Mesh))
			{
				const TCHAR* Path = *CLOTH_PATH;
				FadeoutMesh(ClothMesh, Path, FadeValue);
			}
		}

		TArray<UActorComponent*> WeaponComps = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "WEAPON");
		for (const auto& Mesh : WeaponComps)
		{
			if (USkeletalMeshComponent* WeaponMesh = Cast<USkeletalMeshComponent>(Mesh))
			{
				
				const TCHAR* Path = *WEAPON_PATH;
				FadeoutMesh(WeaponMesh, Path, FadeValue);
			}
		}
	}
}

float UEffectComponent::FadeoutMesh(USkeletalMeshComponent* Mesh, const TCHAR* M_Address, float FadeValue)
{
	//Mesh->GetMaterials()[i]
	//UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, M_Address);

	for (int i = 0; i < Mesh->GetMaterialSlotNames().Num(); i++)
	{
		UMaterialInterface* Material = Mesh->GetMaterials()[i];
		UMaterialInstanceDynamic* DynamicMaterial = Mesh->CreateDynamicMaterialInstance(i, Material);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue("Fadeout", FadeValue);
		}
	}

	return 0.f;
}

void UEffectComponent::AllMeshHidden(bool bAcitvate)
{
	if (ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner()))
	{
		TArray<UActorComponent*> BodyComps = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "BODY");
		for (const auto& Mesh : BodyComps)
		{
			if (USkeletalMeshComponent* BodyMesh = Cast<USkeletalMeshComponent>(Mesh))
			{
				BodyMesh->SetHiddenInGame(bAcitvate);
			}
		}

		TArray<UActorComponent*> ClothComps = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "CLOTH");
		for (const auto& Mesh : ClothComps)
		{
			if (USkeletalMeshComponent* ClothMesh = Cast<USkeletalMeshComponent>(Mesh))
			{
				ClothMesh->SetHiddenInGame(bAcitvate);
			}
		}

		TArray<UActorComponent*> WeaponComps = Owner->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), "WEAPON");
		for (const auto& Mesh : WeaponComps)
		{
			if (USkeletalMeshComponent* WeaponMesh = Cast<USkeletalMeshComponent>(Mesh))
			{
				WeaponMesh->SetHiddenInGame(bAcitvate);
			}
		}
	}
}

void UEffectComponent::RefreshWeaponMesh()
{
	if (ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner()))
	{
		if (Owner->GetHoldType() == EEquipHand::None)
		{
			if (Owner->GetMainWeapon().IsSet())
			{
				Owner->GetMainWeaponMesh()->GetMaterials().Empty();
				for (int i = 0; i < Owner->GetMainWeaponMesh()->GetMaterialSlotNames().Num(); i++)
				{
					Owner->GetMainWeaponMesh()->SetMaterial(i, Owner->GetMainWeapon().ItemData.Mesh->GetMaterials()[i].MaterialInterface);
				}
			}
			if (Owner->GetSecondaryWeapon().IsSet())
			{
				Owner->GetSecondaryWeaponMesh()->GetMaterials().Empty();
				for (int i = 0; i < Owner->GetSecondaryWeaponMesh()->GetMaterialSlotNames().Num(); i++)
				{
					Owner->GetSecondaryWeaponMesh()->SetMaterial(i, Owner->GetSecondaryWeapon().ItemData.Mesh->GetMaterials()[i].MaterialInterface);
				}
			}
		}
		else
		{
			if (Owner->GetHoldTypeWeapon().IsSet())
			{
				Owner->GetMainWeaponMesh()->GetMaterials().Empty();
				for (int i = 0; i < Owner->GetMainWeaponMesh()->GetMaterialSlotNames().Num(); i++)
				{
					Owner->GetMainWeaponMesh()->SetMaterial(i, Owner->GetHoldTypeWeapon().ItemData.Mesh->GetMaterials()[i].MaterialInterface);
				}
			}
			//if (Owner->GetSecondaryWeapon().IsSet())
			//{
			//	Owner->GetSecondaryWeaponMesh()->GetMaterials().Empty();
			//	for (int i = 0; i < Owner->GetSecondaryWeaponMesh()->GetMaterialSlotNames().Num(); i++)
			//	{
			//		Owner->GetSecondaryWeaponMesh()->SetMaterial(i, Owner->GetSecondaryWeapon().ItemData.Mesh->GetMaterials()[i].MaterialInterface);
			//	}
			//}
		}
	}
}

void UEffectComponent::PlayEffect(USoundBase* PlayCue, UNiagaraSystem* NiagaraAsset, float CueVolume, FVector EffectLocation, FRotator EffectRotation, float EffectScale)
{
	if (ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner()))
	{
		if (NiagaraAsset != nullptr)
		{
			if (EffectLocation == FVector::ZeroVector)
			{

				Owner->GetNiagaraComponent()->SetAsset(NiagaraAsset);
				Owner->GetNiagaraComponent()->Activate(true);
			}
			else
			{
				UNiagaraComponent* SpawnNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraAsset, EffectLocation, EffectRotation, FVector(1.0f));
				SpawnNiagara->SetVariableFloat("Scale", EffectScale);

			}

			if (PlayCue != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), PlayCue, EffectLocation, CueVolume);
			}
		}
	}
}

void UEffectComponent::FootStep(FName SocketName, float Volume)
{
	if (ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner()))
	{
		const FVector WorldLocation = Owner->GetMesh()->GetComponentLocation();
		const FVector BreakVector = WorldLocation + FVector(0.f, 0.f, 98.f);

		const FVector SocketLocation = Owner->GetMesh()->GetSocketLocation(SocketName);

		const FVector Start = FVector(SocketLocation.X, SocketLocation.Y, BreakVector.Z);
		const FVector End = Start - FVector(0.f, 0.f, 151.f);
		FHitResult OutHit;

		bool bHit = UKismetSystemLibrary::LineTraceSingle(this,
			Start, End,
			TraceTypeQuery1,
			true,
			{ Owner },
			EDrawDebugTrace::None, OutHit, true);
		if (bHit)
		{
			FEffectData* EffectData = GetSurfaceEffectData(OutHit.PhysMaterial, StepEffectDt);
			if (EffectData) PlayEffect(EffectData->Sound, EffectData->Niagara, Volume, OutHit.ImpactPoint);
		}
	}
}

void UEffectComponent::StepEffect(UPhysicalMaterial* PhysMaterial, FVector PlayLocation)
{
	if (StepEffectDt)
	{
		FEffectData* EffectData = GetSurfaceEffectData(PhysMaterial, StepEffectDt);
		if (EffectData) PlayEffect(EffectData->Sound, EffectData->Niagara, 1.0f, PlayLocation);
	}
}

UFXSystemComponent* UEffectComponent::SpawnNiagara(USkeletalMeshComponent* ApplyMesh, FEffectData* Data, FName SocketName, FName TagName)
{
	if(!Data) return nullptr;

	if (ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner()))
	{
		if (ApplyMesh)
		{
			if (Data->Sound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), Data->Sound, Owner->GetActorLocation());
			}
			if (Data->Niagara)
			{
				UFXSystemComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAttached(
					Data->Niagara,
					ApplyMesh,
					SocketName,
					ApplyMesh->GetSocketLocation(SocketName),
					UKismetMathLibrary::MakeRotFromZ(ApplyMesh->GetSocketLocation(SocketName)), EAttachLocation::KeepWorldPosition, false);

				Component->ComponentTags.AddUnique(TagName);

				return Component;
			}
		}
	}

	return nullptr;
}

void UEffectComponent::RemoveNiagara(USkeletalMeshComponent* ApplyMesh, FName TagName)
{
	if (ApplyMesh)
	{
		TArray<USceneComponent*> Children;
		ApplyMesh->GetChildrenComponents(false, Children);

		if (Children.Num())
		{
			for (USceneComponent* Component : Children)
			{
				if (Component && Component->ComponentHasTag(TagName))
				{
					if (UFXSystemComponent* FXComponent = Cast<UFXSystemComponent>(Component))
					{
						FXComponent->ComponentTags.Remove(TagName);
						FXComponent->Deactivate();
					}
				}
			}
		}
	}
}


FEffectData* UEffectComponent::GetEffectData(FName RowName)
{
	if (EffectDt)
	{
		static const FString ContextString = TEXT("Effect Data Context");
		FEffectData* Row = EffectDt->FindRow<FEffectData>(RowName, ContextString, true);

		return Row != nullptr ? Row : nullptr;
	}
	else
	{
		return nullptr;
	}
}

FEffectData* UEffectComponent::GetSurfaceEffectData(TWeakObjectPtr<class UPhysicalMaterial> PhysMaterial, UDataTable* UseDt)
{
	if (UseDt)
	{
		FName RowName;
		switch (PhysMaterial->SurfaceType)
		{
			case SurfaceType1:
				RowName = TEXT("Wood");
				break;
			case SurfaceType2:
				RowName = TEXT("Concrete");
				break;
			case SurfaceType3:
				RowName = TEXT("Dirt");
				break;
			case SurfaceType4:
				RowName = TEXT("Metal");
				break;
			case SurfaceType5:
				RowName = TEXT("Water");
				break;
			case SurfaceType6:
				RowName = TEXT("Sand");
				break;
			case SurfaceType7:
				RowName = TEXT("Snow");
				break;
			default:
				break;
		}

		static const FString ContextString = TEXT("Step Effect Data Context");
		FEffectData* Row = UseDt->FindRow<FEffectData>(RowName, ContextString, true);

		return Row != nullptr ? Row : nullptr;
	}
	else
	{
		return nullptr;
	}
}
