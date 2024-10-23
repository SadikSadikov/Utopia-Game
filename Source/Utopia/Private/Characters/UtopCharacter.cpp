// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/UtopCharacter.h"
#include "Components/DecalComponent.h"


AUtopCharacter::AUtopCharacter()
{
	Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
	Decal->SetupAttachment(GetRootComponent());
}
