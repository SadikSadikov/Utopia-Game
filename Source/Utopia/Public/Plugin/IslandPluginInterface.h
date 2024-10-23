// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IslandPluginInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIslandPluginInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UTOPIA_API IIslandPluginInterface
{
	GENERATED_BODY()

public:

	// Island Gen
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ScaleUp(float Delay);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IslandGenComplete();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IslandSeed(FRandomStream& Seed);

	//
	virtual void SpawningComplete() = 0;
};
