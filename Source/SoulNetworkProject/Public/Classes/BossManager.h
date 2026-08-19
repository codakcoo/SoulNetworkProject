// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ManagerInterface.h"
#include "BossManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartLevelSequence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndLevelSequence);

UENUM(BlueprintType)
enum class EEnterType : uint8
{
	Trigger		= 0 UMETA(DisplayName = "Trigger"),
	Interaction = 1 UMETA(DisplayName = "Interfaction"),
};

UCLASS()
class SOULNETWORKPROJECT_API ABossManager : public AActor, public IManagerInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	void SetupManager(bool bActive, bool bCinematic);
	UFUNCTION(Server, Reliable)
	void Server_SetupManager(bool bActive, bool bCinemaic);
	UFUNCTION()
	void UpdatedBossManager();
	UFUNCTION()
	void UpdatedBoss(bool bActive);
	UFUNCTION()
	void UpdatedEntrance(bool bActive);
	UFUNCTION()
	void UpdatedTriggers(bool bActive);

	UFUNCTION()
	void SetActivate(bool bActive) { bActivate = bActive; }
	UFUNCTION()
	bool GetActivate() const { return bActivate; }
	UFUNCTION()
	void SetShowedCinemaitc(bool bShowed) { bShowedCinematic = bShowed; }
	UFUNCTION()
	bool GetShowedCinematic() const;
	//UFUNCTION()
	//bool GetShowedCinematic() const { return bShowedCinematic; }
	UFUNCTION()
	class ABossCharacter* GetBossCharacter() const { return Boss; }
	UFUNCTION()
	TArray<class ACharacterBase*> GetInAreaActor() const { return InAreaActors; }
	UFUNCTION()
	FVector GetEnterancesSpawnLocation() const;
	UFUNCTION()
	FRotator GetEnterancesSpawnRotation() const;

	UFUNCTION()
	FVector GetShowedCinematicLocation() const;
	UFUNCTION(BlueprintGetter)
	class ULevelSequencePlayer* GetLSPlayer() const { return LevelSequencePlayer; }

	UFUNCTION()
	void IncreaseAreaActor(class ACharacterBase* ApplyPawn);
	UFUNCTION()
	void DecreaseAreaActor(class ACharacterBase* ApplyPawn);
	UFUNCTION()
	void CheckAreaActors(TArray<class ACharacterBase*> PermuteActors);

	// Interface Function //
	UFUNCTION()
	virtual bool UpdatedAttackInManager(class ACharacterBase* ApplyPawn) override;
	////////////////////////

	UFUNCTION(BlueprintCallable)
	void SingleEffect(bool bActive);
	UFUNCTION(BlueprintCallable)
	void MultiEffect(bool bActive);

	UFUNCTION(BlueprintCallable)
	void StartSequence(class APlayerCharacter* InPlayer);
	UFUNCTION(BlueprintCallable)
	void EndSequence();



private:
	UFUNCTION(Server, Reliable)
	void StartBossStage(class APlayerCharacter* Player);

	UFUNCTION(NetMulticast, Reliable)
	void SetupObjects();

public:
	UPROPERTY(BlueprintAssignable, Category = "LevelSequence")
	FStartLevelSequence StartLevelSequence;
	UPROPERTY(BlueprintAssignable, Category = "LevelSequence")
	FEndLevelSequence EndLevelSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A|Object")
	bool bHideen =false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A|Object")
	class ACinematicObject* CinematicActor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A|Object")
	class ABossCharacter* Boss;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A|Object")
	TArray<class ABossTrigger*> Triggers;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A|Object")
	TArray<class ABossEntrance*> Entrances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Object")
	TArray<class ASmartObject*> EventObjects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Object")
	TArray<class AEffectObject*> EffectObjects;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A|Object")
	class ABossStartPoint* StartPoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A|Object")
	class ASignPointActor* SignPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Data", meta = (MakeEditWidget = "true"))
	FVector ShowedCinematicLocation;

protected:
	UPROPERTY(VisibleAnywhere)
	class ALevelSequenceActor* LevelSequenceActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Sequence")
	class ULevelSequence* LevelSequence;
	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetLSPlayer)
	class ULevelSequencePlayer* LevelSequencePlayer;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Value", meta = (AllowPrivateAccess = "true"), Replicated)
	bool bActivate = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Value", meta = (AllowPrivateAccess = "true"), Replicated)
	bool bShowedCinematic = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "A|Value", meta = (AllowPrivateAccess = "true"))
	EEnterType EnterType;

	UPROPERTY(Replicated)
	TArray<class ACharacterBase*> InAreaActors;

	class AEffectObject* CurrentEffectObject = nullptr;

	FVector CinematicActorLocation = FVector::ZeroVector;
};
