// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Player/UtopPlayerController.h"
#include "Kismet/KismetInputLibrary.h"
#include "Utopia/PrintStrings.h"

AUtopPlayerController::AUtopPlayerController()
{

	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
}

void AUtopPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//FInputModeGameAndUI InputMode;
	//InputMode.SetHideCursorDuringCapture(false);
	//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	//SetInputMode(InputMode);
}

void AUtopPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction(FName("KeyDetect"), EInputEvent::IE_Pressed, this, &AUtopPlayerController::OnKeyDetect);
		InputComponent->BindAxis(FName("MouseMove"), this, &AUtopPlayerController::OnMouseMove);
		
	}
	
}

void AUtopPlayerController::OnKeyDetect(FKey KeyPressed)
{
	if (UKismetInputLibrary::Key_IsGamepadKey(KeyPressed))
	{
		if (InputType != EInputType::EIT_Gamepad)
		{
			//FInputModeGameOnly InputMode;
			//SetInputMode(InputMode);

			InputType = EInputType::EIT_Gamepad;
			//bShowMouseCursor = false;
			KeySwitchDelegate.Broadcast(InputType);
		}

	}
	
}

void AUtopPlayerController::OnMouseMove(float Value)
{

	if (Value != 0.f)
	{
		if (InputType != EInputType::EIT_KeyMouse)
		{
			//FInputModeGameAndUI InputMode;
			//InputMode.SetHideCursorDuringCapture(false);
			//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			//SetInputMode(InputMode);

			InputType = EInputType::EIT_KeyMouse;
			//bShowMouseCursor = true;
			KeySwitchDelegate.Broadcast(InputType);
		}
	}
	

}


