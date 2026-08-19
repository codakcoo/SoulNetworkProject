// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "MasterMagicActor.generated.h"


UENUM(BlueprintType)
enum class ERunType : uint8
{
	Once		UMETA(DisplayName = "Once"),
	Duration	UMETA(DisplayName = "Duration"),
	Loop		UMETA(DisplayName = "Loop")
};

UENUM(BlueprintType)
enum class ESpellStateType : uint8
{
	Begin		UMETA(DisplayName = "Begin"),
	Loop		UMETA(DisplayName = "Loop"),
	End			UMETA(DisplayName = "End")
};

USTRUCT(BlueprintType)
struct FMagicAsset
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Info")
	UNiagaraSystem* BeginNiagaraAsset;
	UPROPERTY(EditAnywhere, Category = "Info")
	UNiagaraSystem* LoopNiagaraAsset;
	UPROPERTY(EditAnywhere, Category = "Info")
	UNiagaraSystem* ReactionNiagaraAsset;

	UPROPERTY(EditAnywhere, Category = "Info")
	class USoundBase* BeginSoundAsset;
	UPROPERTY(EditAnywhere, Category = "Info")
	class USoundBase* LoopSoundAsset;
	UPROPERTY(EditAnywhere, Category = "Info")
	class USoundBase* ReactionSoundAsset;

	UPROPERTY(EditAnywhere, Category = "Info")
	float LifeTime = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Info")
	float IncreaseValue = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Info")
	bool bDetachAtLoop = false;
};

UCLASS()
class SOULNETWORKPROJECT_API AMasterMagicActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasterMagicActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//�⺻���� �߻�ȭ �Լ���// *�ڽ� ��ü�� �������̵� �ؾ���*

	virtual void BeginData(class ACharacterBase* InOwner, class USkeletalMeshComponent* CastMesh) PURE_VIRTUAL(AMasterMagicActor, );	//Begin�� ȣ�� �Լ�//

	virtual void LoopData() PURE_VIRTUAL(AMasterMagicActor, );	//Loop�� ȣ�� �Լ�//

	virtual void MultipleData(class ACharacterBase* InOwner) PURE_VIRTUAL(AMasterMagicActor, );	//Multiple ȣ�� �Լ�// *��Ȳ�� ���� ������ �ϰų� ���ص� ��*

	virtual void EndData() PURE_VIRTUAL(AMasterMagicActor, );	//End�� ȣ�� �Լ�//

protected:
	//���� Niagara, ��ġ, ȸ���ϰų� Detach�Ҷ��� ȣ���ؼ� �����.//
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_BeginData();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_LoopData();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_DetachedSetLocation(FVector NewLocation);
	////////////////////////////////////

	UFUNCTION()
	void PointReaction(FHitResult OutHit);
	UFUNCTION()
	void RadialReaction(FTransform SpawnTransform, float Radius);

	//�ݸ��� Update// Tick.
	virtual	void UpdatedCollisionEvent(float InDeltaTime) PURE_VIRTUAL(AMasterMagicActor, );
	UFUNCTION(Server, Reliable)
	void Server_UpdatedCollisionEvent(float InDeltaTime);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_Effect(FTransform SpawnTransform);

	UFUNCTION()
	void UpdatedEndScale(float InDeltaTime);

	UFUNCTION()
	float GetFloatNiagaraParam(FName VariableName);


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	ERunType RunType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FMagicAsset SpellData;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageType")
	TSubclassOf<class UDamageType> MagicDamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* Niagara;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* Audio;

	

	FTimerHandle LifeTimer;

	UPROPERTY(Replicated)
	class ACharacterBase* SpawnOwner;
	UPROPERTY(Replicated)
	TArray<AActor*> IgnoreActors;


	UPROPERTY(Replicated)
	ESpellStateType SpellStateType;
};
