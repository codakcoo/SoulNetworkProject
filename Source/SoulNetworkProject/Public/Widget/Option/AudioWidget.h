// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AudioWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UAudioWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnMasterValueChanged(float Value);
	UFUNCTION()
	void OnMusicValueChanged(float Value);
	UFUNCTION()
	void OnSFXValueChanged(float Value);
	UFUNCTION()
	void OnVoiceValueChanged(float Value);

	UFUNCTION()
	void CallSaveValue();

public:
	UPROPERTY(meta = (BindWidget))
	class USlider* Master_Slider;
	UPROPERTY(meta = (BindWidget))
	class USlider* Music_Slider;
	UPROPERTY(meta = (BindWidget))
	class USlider* SFX_Slider;
	UPROPERTY(meta = (BindWidget))
	class USlider* Voice_Slider;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class USoundClass* Master_Class;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mix", meta = (AllowPrivateAccess = "true"))
	class USoundMix* Master_Mix;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class USoundClass* Music_Class;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mix", meta = (AllowPrivateAccess = "true"))
	class USoundMix* Music_Mix;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class USoundClass* SFX_Class;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mix", meta = (AllowPrivateAccess = "true"))
	class USoundMix* SFX_Mix;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class USoundClass* Voice_Class;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mix", meta = (AllowPrivateAccess = "true"))
	class USoundMix* Voice_Mix;

};
