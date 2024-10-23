// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtopShareFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UTOPIA_API UUtopShareFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION()
	static FVector ConvertToSteppedPos(const FVector& A);
	
};
