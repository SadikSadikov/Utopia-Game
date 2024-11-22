// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtopFunctionLibrary.generated.h"

class AUtopGameModeBase;
class UUtopGameInstance;
class UCombatComponent;
class UAttributeComponent;

UCLASS()
class UTOPIA_API UUtopFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static FVector SteppedPosition(const FVector& NewParam);

	UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "WorldContextObject"))
	static AUtopGameModeBase* GetUtopGameMode(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (DefaultToSelf = "WorldContextObject"))
	static UUtopGameInstance* GetUtopGameInstance(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	static UCombatComponent* GetCombatComponentByClass(AActor* Owner);

	UFUNCTION(BlueprintPure)
	static UAttributeComponent* GetAttributeComponentByClass(AActor* Owner);

	
};
