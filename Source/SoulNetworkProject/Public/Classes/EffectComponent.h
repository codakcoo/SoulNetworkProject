// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulNetworkProject/BuffState.h"
#include "Engine/DataTable.h"
#include "EffectComponent.generated.h"

UENUM(BlueprintType)
enum class EEffectType : uint8
{
	Damaged		UMETA(DisplayName = "Damaged"),
	Block		UMETA(DisplayName = "Block"),
	Health		UMETA(DisplayName = "Health"),
	Mana		UMETA(DisplayName = "Mana"),
	Status		UMETA(DisplayName = "Status"),
	Surface		UMETA(DisplayName = "Surface"),
};

USTRUCT(BlueprintType)
struct FEffectData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UNiagaraSystem* Niagara;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USoundBase* Sound;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULNETWORKPROJECT_API UEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEffectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/*SurfaceType�� EffectType�� surface�϶��� �۵���*/
	UFUNCTION()
	void EffectSequence(EEffectType EffectType, FVector EffectLocation = FVector::ZeroVector, FHitResult HitResult = FHitResult());

	UFUNCTION()
	void WeaponTrail(EEquipHand HandType, bool bActive);


	UFUNCTION()
	void RunBodyMeshNiagara(FName BuffName);
	UFUNCTION()
	void RemoveBodyMeshNiagara(FName TagName);
	UFUNCTION()
	void RunWeaopnMeshNiagara(FName BuffName);
	UFUNCTION()
	void RemoveWeaponMeshNigara();

	UFUNCTION()
	void AllMeshFadeout(float FadeValue);
	float FadeoutMesh(class USkeletalMeshComponent* Mesh, const TCHAR* M_Address, float FadeValue);

	UFUNCTION()
	void AllMeshHidden(bool bAcitvate);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_BodyMeshNigara(EBuffElementType EffectType, float Delay);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemoveBodyMesh(class USkeletalMeshComponent* Mesh, FName TagName);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_RemoveBodyMesh(class USkeletalMeshComponent* Mesh, FName TagName);

	FORCEINLINE FString GetBodyMeshPath() const { return BODY_PATH; }

	UFUNCTION()
	void RefreshWeaponMesh();

	UFUNCTION()
	void PlayEffect(class USoundBase* PlayCue = nullptr, class UNiagaraSystem* NiagaraAsset = nullptr, float CueVolume = 1.0f, FVector EffectLocation = FVector::ZeroVector, FRotator EffectRotation = FRotator::ZeroRotator, float EffectScale = 1.0f);

	UFUNCTION()
	void FootStep(FName SocketName, float Volume);

	UFUNCTION()
	void StepEffect(class UPhysicalMaterial* PhysMaterial, FVector PlayLocation);

	//��ȣ�ۿ� ����Ʈ//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ReactionEffect(class USoundBase* PlayCue = nullptr, class UNiagaraSystem* NiagaraAsset = nullptr, FVector EffectLocation = FVector::ZeroVector, FRotator EffectRotation = FRotator::ZeroRotator, float EffectScale = 1.0f);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_ReactionEffect(class USoundBase* PlayCue = nullptr, class UNiagaraSystem* NiagaraAsset = nullptr, FVector EffectLocation = FVector::ZeroVector, FRotator EffectRotation = FRotator::ZeroRotator, float EffectScale = 1.0f);
	////////////////////

private:
	class UFXSystemComponent* SpawnNiagara(USkeletalMeshComponent* ApplyMesh, FEffectData* Data, FName SocketName, FName TagName);

	void RemoveNiagara(USkeletalMeshComponent* ApplyMesh, FName TagName);

	//����Ʈ//
	FEffectData* GetEffectData(FName RowName);

	FEffectData* GetSurfaceEffectData(TWeakObjectPtr<class UPhysicalMaterial> PhysMaterial, UDataTable* UseDt);



protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DT")
	class UDataTable* EffectDt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DT")
	class UDataTable* StepEffectDt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	class UNiagaraSystem* WeaponTraceNigara_Obj;

	//Mesh PATH//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
		FString BODY_PATH;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
		FString CLOTH_PATH;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
		FString WEAPON_PATH;
	/////////////


private:
	FTimerHandle OpacityTimer;
	FTimerHandle NiagaraTimer;
};
