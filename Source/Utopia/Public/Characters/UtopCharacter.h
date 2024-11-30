// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/UtopBaseCharacter.h"
#include "Characters/Interfaces/CharacterBasePlaceInterface.h"
#include "UtopCharacter.generated.h"

class UDecalComponent;

UCLASS()
class UTOPIA_API AUtopCharacter : public AUtopBaseCharacter, public ICharacterBasePlaceInterface
{
	GENERATED_BODY()

public:

	AUtopCharacter();

	/* CharacterBasePlaceInterface */

	virtual void GetCharacterBasePlace_Implementation(AActor*& OutBase);

	/* end CharacterBasePlaceInterface */

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> Base;

	
};
