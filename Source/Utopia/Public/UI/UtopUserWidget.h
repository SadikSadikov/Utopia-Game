// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UtopUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UTOPIA_API UUtopUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetWidgetOwner(AActor* InWidgetOwner);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> WidgetOwner;

	UFUNCTION(BlueprintImplementableEvent)
	void WidgetOwnerSet();
	
};
