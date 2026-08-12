// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "FriendlyQueryContext.generated.h"

/**
 * 
 */
UCLASS()
class SOULNETWORKPROJECT_API UFriendlyQueryContext : public UEnvQueryContext
{
	GENERATED_BODY()
	
protected:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
