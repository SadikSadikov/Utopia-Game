// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/UtopBaseInteractable.h"
#include "Core/GameMode/UtopResourceInterface.h"
#include "Plugin/IslandPluginInterface.h"
#include "Utopia/Public/UtopTypes/Resource.h"
#include "UtopBaseResource.generated.h"

/**
 * 
 */
UCLASS()
class UTOPIA_API AUtopBaseResource : public AUtopBaseInteractable, public IUtopResourceInterface,
	public IIslandPluginInterface
{
	GENERATED_BODY()

public:

	AUtopBaseResource();


	/* UtopResourceInterface */

	virtual void RemoveResource_Implementation(EResourceType& TargetResource, int32& Value) override;

	virtual void AddResource_Implementation(EResourceType TargetResource, int32 Value) override;

	virtual void GetCurrentResources_Implementation(TMap<EResourceType, int32>& InResources) override;

	virtual void RemoveTargetResources_Implementation(EResourceType TargetResource, int32 Value) override;

	virtual void CheckResource_Implementation(EResourceType TargetResource, bool& bCheck, int32& Num) override;

	/* end UtopResourceInterface */


	/** IslandPluginInterface */

	virtual void ScaleUp_Implementation(float Delay) override;

	virtual void IslandGenComplete_Implementation() override;

	virtual void IslandSeed_Implementation(FRandomStream& Seed) override;

	virtual void SpawningComplete() override;

	/** end IslandPluginInterface */


protected:

	virtual void BeginPlay() override;


	virtual void Interact(float& Delay) override;

	virtual void SetProgressionsState(float Progression) override;

	// Resource Properties

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ResourceProperties")
	EResourceType ResourceType = EResourceType::ERT_Food;

	UPROPERTY(EditAnywhere, Category = "ResourceProperties")
	float ResourceAmount = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ResourceProperties")
	float CollectionTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "ResourceProperties")
	int32 CollectionValue = 10;

	// Progression

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Progression")
	bool bUseRandomMesh;

	UFUNCTION()
	void OnTimelineUpdate(float Value);

private:

	void OnScaleUpDelayCompleted();

	void Death();

	
};
