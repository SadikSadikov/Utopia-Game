#pragma once


#include "CoreMinimal.h"
#include "Weapon.generated.h"

class AUtopWeapon;

USTRUCT(BlueprintType)
struct FWeaponParams
{
	GENERATED_BODY()

public:

	FWeaponParams(){}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AUtopWeapon> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SocketName = FName();



};