// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Classes/InteractionActor.h"
#include "Interface/InteractableInterface.h"
#include "SignPointActor.generated.h"

/**
 * 
 */
USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUser;
};

UCLASS()
class SOULNETWORKPROJECT_API ASignPointActor : public AInteractionActor, public IInteractableInterface
{
	GENERATED_BODY()
	
protected:
	ASignPointActor();

	virtual void BeginPlay() override;

	//Interface//
	UFUNCTION()
	virtual void Interact(class APlayerCharacter* Character) override;

	UFUNCTION()
	virtual FName GetInteractableName() const override;

	UFUNCTION()
	virtual void UpdatedOverlapEnd() override;

	UFUNCTION()
	virtual void PlayInteractionMt() override;
	/////////////

public:
	UFUNCTION()
	void SetServerRow(TArray<FServerData> ServerNames);
	UFUNCTION()
	void CreateServerRow(const FServerData Data, uint8 Index);
	UFUNCTION()
	void DestroyServerRow();

	UFUNCTION()
	void SetupWalls(bool bActive);
	UFUNCTION()
	void SwitchSearchEffect(bool bActive);
	UFUNCTION()
	void SwitchSearchWall(bool bAcitve);

	UFUNCTION()
	void RefreshObject(bool bAcitve);

	UFUNCTION()
	TArray<class ASignPointBlock*> GetWalls() const { return Walls; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* Niagara;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UAudioComponent* Audio;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	class UNiagaraSystem* Searching_NS;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	class UNiagaraSystem* Searched_NS;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	class USoundBase* Searching_SC;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	class USoundBase* Searched_SC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	class UAnimMontage* InteractionMt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall")
	TArray<class ASignPointBlock*> Walls;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Server", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AServerRowActor> ServerClass;

	TArray<class AServerRowActor*> Servers;

	class APlayerCharacter* CurrentOwner = nullptr;
	class UInteractionWidget* CurrnetWidget = nullptr;
};
