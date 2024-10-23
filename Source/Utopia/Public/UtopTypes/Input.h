#pragma once

UENUM(BlueprintType)
enum class EInputType : uint8
{
	EIT_Unknown UMETA(DisplayName = "Unknown"),
	EIT_KeyMouse UMETA(DisplayName = "KeyMouse"),
	EIT_Gamepad UMETA(DisplayName = "Gamepad")

};
