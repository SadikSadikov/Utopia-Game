// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UtopVillagerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UUtopVillagerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UTOPIA_API IUtopVillagerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeJob(FName NewJob);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayDeliverAnim(float& OutDelay);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayWorkAnim(float Delay);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ProgressBuilding(float InvestedTimer, float& Out);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Action(AActor* NewParam);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnToDefaultBT();
};
