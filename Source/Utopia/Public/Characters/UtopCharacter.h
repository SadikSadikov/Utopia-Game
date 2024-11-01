// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/UtopBaseCharacter.h"
#include "UtopCharacter.generated.h"

class UDecalComponent;

UCLASS()
class UTOPIA_API AUtopCharacter : public AUtopBaseCharacter
{
	GENERATED_BODY()

public:

	AUtopCharacter();

protected:

	virtual void BeginPlay() override;

private:

	
};
