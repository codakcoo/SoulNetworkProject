// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GenericPlatform/GenericPlatform.h"
#include "MenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SOULNETWORKPROJECT_API IMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Functions)
	UFUNCTION()
	virtual void Host(FName ServerName) = 0;
	UFUNCTION()
	virtual void Join(uint32 Index) = 0;
	UFUNCTION()
	virtual void Close() = 0;
	UFUNCTION()
	virtual void StartSession() = 0;
	
	UFUNCTION()
	virtual void BackMainMenu() = 0;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Functions)
	void RefreshServerList();
	UFUNCTION()
	virtual void RefreshServerTravel() = 0;

	UFUNCTION()
	virtual class UTexture2D* SetupCharacterImage(FString SaveName, uint8 SaveIndex) = 0;

	UFUNCTION()
	virtual void SearchServerList(FName SessionName) = 0;

	UFUNCTION()
	virtual class UClientInfo* GetClient() = 0;
	UFUNCTION()
	virtual uint8 GetUserLevel(FString UserName) = 0;
	UFUNCTION()
	virtual bool GetOpenSession() = 0;
	UFUNCTION()
	virtual bool GetStartSession() = 0;
	UFUNCTION()
	virtual FName GetCurrentServerName() = 0;

	UFUNCTION()
	virtual void SetSignPoint(class ASignPointActor* InPointActor) = 0;
	UFUNCTION()
	virtual class ASignPointActor* GetSignPoint() const = 0;

	UFUNCTION()
	virtual void ScreenFade(bool bFadeIn) = 0;

};
