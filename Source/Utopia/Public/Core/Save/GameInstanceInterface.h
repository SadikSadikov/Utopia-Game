// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Utopia/Public/UtopTypes/Resource.h"
#include "Utopia/Public/UtopTypes/Save.h"
#include "GameInstanceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

class UUtopSaveGame;

class UTOPIA_API IGameInstanceInterface
{
	GENERATED_BODY()

	
public:

	// Retrieve Save Data

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CheckSaveBool();

	virtual FRandomStream GetSeed() = 0;

	virtual TArray<FSaveInteract> GetAllInteractables() = 0;

	virtual UUtopSaveGame* GetSave() = 0;

	// Set Save Data

	virtual void StartNewLevel() = 0;

	virtual void SaveGame() = 0;

	virtual void UpdateSaveData() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateAllInteractables();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateAllResource(const TMap<EResourceType, int32>& InResources);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateAllVillagers();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ClearSave(bool bClearSeed);

	//

	virtual void LoadLevel() = 0;


};
