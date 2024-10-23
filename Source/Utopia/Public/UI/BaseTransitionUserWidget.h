// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseTransitionUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UTOPIA_API UBaseTransitionUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TransIn();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TransOut();
	
};
