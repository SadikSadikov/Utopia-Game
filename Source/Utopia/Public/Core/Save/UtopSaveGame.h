// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Utopia/Public/UtopTypes/Resource.h"
#include "Utopia/Public/UtopTypes/Save.h"
#include "UtopSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class UTOPIA_API UUtopSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FRandomStream Seed;

	UPROPERTY()
	TMap<EResourceType, int32> Resources;

	UPROPERTY()
	TArray<FSaveInteract> Interactables;

	UPROPERTY()
	TArray<FVillager> Villagers;

	UPROPERTY()
	float PlayTime;

};
