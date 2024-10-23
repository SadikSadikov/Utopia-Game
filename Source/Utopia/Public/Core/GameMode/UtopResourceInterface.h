// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Utopia/Public/UtopTypes/Resource.h"
#include "UtopResourceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUtopResourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UTOPIA_API IUtopResourceInterface
{
	GENERATED_BODY()

public:

	// Resource Management

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveResource(EResourceType& TargetResource, int32& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddResource(EResourceType TargetResource, int32 Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetCurrentResources(TMap<EResourceType, int32>& InResources);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveTargetResources(EResourceType TargetResource, int32 Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckResource(EResourceType TargetResource, bool& bCheck, int32& Num);

};
