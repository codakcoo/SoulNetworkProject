// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SoulNetworkProject/ItemState.h"
#include "SoulNetworkProject/ItemData.h"
#include "Classes/InventoryComponent.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
 class USoundBase;

 UENUM(BlueprintType)
 enum class EActiveMenu : uint8
 {
	PlayerMenu		UMETA(DisplayName = "PlayerMenu"),
	InGameMenu		UMETA(DisplayName = "InGameMenu"),
	CheckPointMenu	UMETA(DisplayName = "CheckPointMenu"),
	Popup			UMETA(DisplayName = "Popup"),
	Cinematic		UMETA(DisplayName = "Cinematic")
 };

 UENUM(BlueprintType)
 enum class EEventUIType : uint8
 {
	FELLED		UMETA(DisplayName = "FELLED"),
	DEAD		UMETA(DisplayName = "DEAD")
 };

UCLASS()
class SOULNETWORKPROJECT_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	APlayerHUD();

	virtual void DrawHUD() override;
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

public:
	UFUNCTION()
	void UpdatedState();

	UFUNCTION()
	void SetupInteractionBox(bool bVisible, FText InText = FText());
	UFUNCTION()
	void ShowItemViewList(FInventoryData& Item);

	UFUNCTION()
	void BindBossWidget(bool bVisible);

	UFUNCTION(BlueprintCallable)
	void OnOffInGame();


	UFUNCTION()
	void SetupMenus(class APlayerCharacter* InPawn);

	//Get MainMenu InventoryWidget Class//
	UFUNCTION()
	UWidget* GetInventorySlot();
	UFUNCTION()
	UWidget* GetEquipmentSlot();
	UFUNCTION()
	UWidget* GetSpellSlot();
	UFUNCTION()
	UWidget* GetStorageSlot();
	////////////

	UFUNCTION()
	void ClearPopupWidget();

	//UFUNCTION()
	//void SwitchingUseItemSlot(class AMasterPickupActor* UseItem, bool bWidgetAnim, EEquipHand  HandType = EEquipHand::None);

	//Player Wiget UseItemSlot//
	////////////////////////////

	//Input Change Menu//
	UFUNCTION(BlueprintCallable)
	void TotalSwitchMenu(bool bForward);
	UFUNCTION(BlueprintCallable)
	void TotalReverseMenu();
	UFUNCTION(BlueprintCallable)
	void TotalApplyMenu();
	////////////////////

	//Test FUNCTION//
	UFUNCTION()
	void OnUpdatedHealth();

	UFUNCTION()
	void CallEventUI(EEventUIType EventType);

	UFUNCTION()
	void CallMessageUI(FName Message);

	UFUNCTION()
	bool GetBossBoxActivate() const;
	UFUNCTION()
	bool GetAllSetupMenu() const;
	UFUNCTION()
	EActiveMenu GetActiveMenu() const { return ActiveMenu; }
	UFUNCTION()
	void SetActiveMenu(EActiveMenu ActiveType) { ActiveMenu = ActiveType; }

	UFUNCTION()
	void SetupEquipment();
	UFUNCTION()
	void ClearWidgetTimer();

	UFUNCTION()
	void SwitchCheckPointMenu();

	UFUNCTION()
	bool GetActivateMenu();

	UFUNCTION()
	void ShowPopupMenu(class AActor* ParentActor, TSubclassOf<UUserWidget> PopupClass);
	
	UFUNCTION()
	void SetPressedOnCinematicMode(bool bActive);
	UFUNCTION()
	void EnableCinematicMode();
	UFUNCTION()
	void DiableCinematicMode();

	UFUNCTION()
	void SetServerTimer(bool bActivate);
	UFUNCTION()
	void UpdateServerTimer(float InTimer);

	UFUNCTION()
	void PlayWarpMotion();



public:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> InGameWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> CheckPointWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> CinematicWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> MousePopupWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerWidget")
	FLinearColor FELLEDTint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerWidget")
	FLinearColor DEADTint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerWidget")
	USoundBase* FELLEDSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerWidget")
	USoundBase* DEADSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	class UAnimMontage* GetupMt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	class UAnimMontage* WarpMt;

private:
	class UPlayerWidget* PlayerWidget;
	class UInGameMenu* InGameWidget;
	class UCheckPointMenu* CheckPointWidget;
	class UCinematicWidget* CinematicWidget;

	class UMousePopup* MousePopupWidget;
	class USmartObjectInteraction* PopupWidget = nullptr;
	
	//UPROPERTY(BlueprintGetter = GetPawn)
	class APlayerCharacter* Owner;

	EActiveMenu ActiveMenu = EActiveMenu::PlayerMenu;
};
