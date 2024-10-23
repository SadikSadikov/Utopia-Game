// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UtopLayerGameActivatableWidget.generated.h"

/**
 * 
 */
UCLASS()
class UTOPIA_API UUtopLayerGameActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AddStackItem(TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PullCurrentActiveWidget();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EndGame(bool bWin);
};
