// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UtopBaseCharacter.generated.h"

UCLASS()
class UTOPIA_API AUtopBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AUtopBaseCharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

private:	




};
