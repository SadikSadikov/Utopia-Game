// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UtopTypes/Input.h"
#include "UtopPlayerController.generated.h"

class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKeySwitchSignature, EInputType, NewInput);

UCLASS()
class UTOPIA_API AUtopPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AUtopPlayerController();

	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintAssignable)
	FKeySwitchSignature KeySwitchDelegate;

	UPROPERTY(BlueprintReadOnly)
	EInputType InputType;

protected:

	void OnKeyDetect(FKey KeyPressed);

	void OnMouseMove(float Value);

	virtual void BeginPlay() override;

private:


	UPROPERTY(EditAnywhere)
	TObjectPtr<UUserWidget> CursorWidget;

	
};
