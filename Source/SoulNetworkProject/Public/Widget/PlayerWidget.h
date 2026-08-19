// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoulNetworkProject/ItemData.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPlayerWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION()
	bool InteractionEnabled();
	UFUNCTION()
	ESlateVisibility IsActiveInGameMenu();

	UFUNCTION()
	float GetHealthPercent();
	UFUNCTION()
	float GetManaPercent();
	UFUNCTION()
	float GetStaminaPercent();

	UFUNCTION()
	void SetInteractionText(FText InText);
	UFUNCTION()
	ESlateVisibility IsTwiceToOverlap();

	UFUNCTION()
	void OnInteractionBox();
	UFUNCTION()
	void OffInteractionBox();

	UFUNCTION()
	void SwitchBossBox(bool bVisible);
	UFUNCTION()
	float CheckBossHpValue();
	UFUNCTION()
	FText UpdatedDecreaseHealth();

	UFUNCTION()
	void RefreshItemViewList(FInventoryData Item);

	FORCEINLINE class UPanelWidget* GetViewList() const { return ViewList; }

	UFUNCTION()
	FText UpdatedExpendableText();
	UFUNCTION()
	FText UpdatedMagicText();

	UFUNCTION()
	void PlayEventUI(FText Text, FLinearColor Tint);
	UFUNCTION()
	void CallMessageBox(FText Text);

	UFUNCTION()
	UWidget* BossUI() const { return BossState_Box; }

	UFUNCTION()
	void PlayWidgetAnim() { PlayAnimation(UIBlink); }
	UFUNCTION()
	void BindPawn(class APlayerCharacter* InPawn);

	UFUNCTION()
	void UpdatedSlot(EItemType SlotType, EEquipHand HandType, FInventoryData ItemData, bool bAnim);

	UFUNCTION()
	void UpdatedSoulText(int32 InSoul, bool bAnim);
	UFUNCTION()
	void UpdateHealthText(bool bAnim);
	UFUNCTION()
	void UpdateManaText(bool bAnim);
	UFUNCTION()
	void UpdateStaminaText(bool bAnim);

	UFUNCTION(BlueprintCallable)
	void PlayUpdatedSoul();

	UFUNCTION()
	void SetTimerVisibility(bool bActivate);
	UFUNCTION()
	void UpdateTimer(float InTimer);

private:
	UFUNCTION()
	bool CheckLSlotEnabled();
	UFUNCTION()
	bool CheckRSlotEnabled();
	UFUNCTION()
	bool CheckSpellSlotEnabled();


public:
	//Player StateBar//
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ManaBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBackBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ManaBackBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBackBar;
	///////////////////

	//Interaction Box//
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UVerticalBox* InteractionBox;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractionText;
	UPROPERTY(meta = (BindWidget))
	class UWidget* ActionBox;
	///////////////////

	//Text//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MagicText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ExpendableText;
	///////

	//Item Slot//
	UPROPERTY(meta = (BindWidget))
	class UUseItemWidget* SpellSlot;
	UPROPERTY(meta = (BindWidget))
	class UUseItemWidget* ExpendableSlot;
	UPROPERTY(meta = (BindWidget))
	class UUseItemWidget* L_WeaponSlot;
	UPROPERTY(meta = (BindWidget))
	class UUseItemWidget* R_WeaponSlot;

	UPROPERTY(meta = (BindWidget))
	class UUseItemWidget* FirstArrowSlot;
	UPROPERTY(meta = (BindWidget))
	class UUseItemWidget* SecondArrowSlot;
	//////////////

	//Boss state//
	UPROPERTY(meta = (BindWidget))
	class UWidget* BossState_Box;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* Boss_HpBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Boss_Name;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Boss_DecreaseCount;
	//////////////

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ViewList;

	//Event UI//
	UPROPERTY(meta = (BindWidget))
	class UWidget* EventBox;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* EventText;
	////////////

	//Message//
	UPROPERTY(meta = (BindWidget))
	class UWidget* MessageBox;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MessageText;
	///////////

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AddSoulText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TimerText;

	//Wiget Animation//
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* InteractionVisible;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* InteractionHidden;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* UIBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* EventBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* MessageBlink;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* AddSoul;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* TimerOn;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* TimerOff;
	////////////////////

	uint64 CurrentSoul = 0;
	uint64 OwnSoul = 0;

	float CurrentHealth = 0.0f;
	float CurrentMana = 0.0f;
	float CurrentStamina = 0.0f;
protected:
	TSubclassOf<class UUserWidget> ViewRowClass;

	class APlayerCharacter* Owner;

	bool bUpdateSoul = false;
	bool bUpdateHealth = false;
	bool bUpdateMana = false;
	bool bUpdateStamina = false;

	float SoulAlpha = 0.0f;
	float HealthAlpha = 0.0f;
	float ManaAlpha = 0.0f;
	float StaminaAlpha = 0.0f;
};
