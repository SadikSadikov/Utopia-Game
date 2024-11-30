// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/UtopCharacter.h"



AUtopCharacter::AUtopCharacter()
{
}

void AUtopCharacter::GetCharacterBasePlace_Implementation(AActor*& OutBase)
{
	if (Base)
	{
		OutBase = Base;
	}

	OutBase = nullptr;
}

void AUtopCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add("Character");
}
