// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/SmartObjectInteraction.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
 UENUM(BlueprintType)
 enum class EMMType : uint8
 {
	 None		UMETA(DisplayName = "None"),
	 Host		UMETA(DisplayName = "Host"),
	 Join		UMETA(DisplayName = "Join"),
 };

 UENUM(BlueprintType)
	 enum class EHMType : uint8
 {
	 None		UMETA(DisplayName = "None"),
	 Teamup		UMETA(DisplayName = "Teamup"),
	 Invasion		UMETA(DisplayName = "Invasion"),
 };

UCLASS()
class SOULNETWORKPROJECT_API UInteractionWidget : public USmartObjectInteraction
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

public:
	UFUNCTION()
	virtual bool SetupData(class AActor* InParent) override;

	UFUNCTION()
	void SetServerParent(class AServerRowActor* InParent);

	UFUNCTION()
	void MMHostClicked();
	UFUNCTION()
	void MMJoinClicked();

	UFUNCTION()
	void HMTeamupClicked();
	UFUNCTION()
	void HMInvasionClicked();

	UFUNCTION()
	void CancelClicked();

	UFUNCTION()
	void YesClicked();

	UFUNCTION()
	FText UpdatedDescriptorText();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Descriptor")
	FString OpenSessionDescriptor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Descriptor")
	FString StartSessionDescriptor;

private:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitch;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MMSelectBox;
	UPROPERTY(meta = (BindWidget))
	class UWidget* HMSelectBox;

	//MM Widget//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerNameText;
	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* NumofPlayerText;

	UPROPERTY(meta = (BindWidget))
	class UButton* MM_HostButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* MM_JoinButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* MM_CancelButton;
	/////////////

	//HM Widget//
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HM_Text;

	UPROPERTY(meta = (BindWidget))
	class UButton* HM_TeamupButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* HM_InvasionButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* HM_CancelButton;
	/////////////

	//Confirm Widget//
	UPROPERTY(meta = (BindWidget))
	class UWidget* ConfirmBox;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DescriptorText;
	UPROPERTY(meta = (BindWidget))
	class UButton* YesButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* NoButton;
	/////////////////

	UPROPERTY(meta = (BindWidget))
	class UButton* BGButton;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* BlinkUI;

	class AActor* Parent;

	EMMType MMType;
	EHMType HMType;



};
