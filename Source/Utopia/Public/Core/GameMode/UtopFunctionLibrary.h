// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtopFunctionLibrary.generated.h"

class AUtopGameModeBase;
class UUtopGameInstance;

UCLASS()
class UTOPIA_API UUtopFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static FVector SteppedPosition(const FVector& NewParam);

	UFUNCTION(BlueprintPure)
	static AUtopGameModeBase* GetUtopGameMode(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	static UUtopGameInstance* GetUtopGameInstance(UObject* WorldContextObject);

	
};
