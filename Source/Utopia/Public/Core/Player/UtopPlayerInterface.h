// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UtopTypes/Resource.h"
#include "UtopPlayerInterface.generated.h"

class AUtopBaseInteractable;
class UCommonActivatableWidget;

UINTERFACE(MinimalAPI)
class UUtopPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UTOPIA_API IUtopPlayerInterface
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BeginBuild(TSubclassOf<AUtopBaseInteractable> TargetClass, const TMap<EResourceType, int32>& InResourceCost);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchToBuildMode(bool bSwitchToBuildMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddUI(TSubclassOf<UCommonActivatableWidget> NewParam);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveCurretUI_Layer();

};
