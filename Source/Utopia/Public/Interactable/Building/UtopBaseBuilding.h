// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/UtopBaseInteractable.h"
#include "UtopBaseBuilding.generated.h"

/**
 * 
 */
UCLASS()
class UTOPIA_API AUtopBaseBuilding : public AUtopBaseInteractable
{
	GENERATED_BODY()

public:

	virtual void Interact(float& Delay) override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ProgressConstruct(float InvestedTime, float& RemaniningConstructionTme);

	void SpawnInBuildMode(float Progression);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build", meta = (DisplayName = "ConstructionComplete"))
	void ReceiveConstructionComplete();

	virtual void ConstructionComplete();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build")
	int32 CurrentStage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build")
	float BuildDifficulty = 1.f;

private:


	
};
