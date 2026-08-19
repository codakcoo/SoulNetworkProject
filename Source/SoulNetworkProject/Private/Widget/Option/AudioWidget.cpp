// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Option/AudioWidget.h"
#include "SaveGame/ClientInfo.h"
#include "Interface/SaveInterface.h"

#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

bool UAudioWidget::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	Master_Slider->OnValueChanged.AddDynamic(this, &UAudioWidget::OnMasterValueChanged);
	Music_Slider->OnValueChanged.AddDynamic(this, &UAudioWidget::OnMusicValueChanged);
	SFX_Slider->OnValueChanged.AddDynamic(this, &UAudioWidget::OnSFXValueChanged);
	Voice_Slider->OnValueChanged.AddDynamic(this, &UAudioWidget::OnVoiceValueChanged);

	return true;
}

void UAudioWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const UClientInfo* OptionDataRow = nullptr;
	if (GetGameInstance()->GetClass()->ImplementsInterface(UMenuInterface::StaticClass()))
	{
		OptionDataRow = Cast<IMenuInterface>(GetGameInstance())->GetClient();
	}

	if(!OptionDataRow) return;

	Master_Slider->SetValue(OptionDataRow->MasterVolumeValue);
	Music_Slider->SetValue(OptionDataRow->MusicVolumeValue);
	SFX_Slider->SetValue(OptionDataRow->SFXVolumeValue);
	Voice_Slider->SetValue(OptionDataRow->VoiceVolumeValue);

	OnMasterValueChanged(OptionDataRow->MasterVolumeValue);
	OnMusicValueChanged(OptionDataRow->MusicVolumeValue);
	OnSFXValueChanged(OptionDataRow->SFXVolumeValue);
	OnVoiceValueChanged(OptionDataRow->VoiceVolumeValue);
}

void UAudioWidget::OnMasterValueChanged(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Call Master"));
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), Master_Mix, Master_Class, Value);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), Master_Mix);

	CallSaveValue();
}

void UAudioWidget::OnMusicValueChanged(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Call Music"));
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), Music_Mix, Music_Class, Value);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), Music_Mix);

	CallSaveValue();
}

void UAudioWidget::OnSFXValueChanged(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Call SFX"));
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SFX_Mix, SFX_Class, Value);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), SFX_Mix);

	CallSaveValue();
}

void UAudioWidget::OnVoiceValueChanged(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Call Voice"));
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), Voice_Mix, Voice_Class, Value);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), Voice_Mix);

	CallSaveValue();
}

void UAudioWidget::CallSaveValue()
{
	if (GetGameInstance()->GetClass()->ImplementsInterface(USaveInterface::StaticClass()))
	{
		Cast<ISaveInterface>(GetGameInstance())->SaveAudioSettings(Master_Slider->GetValue(), Music_Slider->GetValue(), SFX_Slider->GetValue(), Voice_Slider->GetValue());
	}
}
