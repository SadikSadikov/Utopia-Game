// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Utopia/Public/UtopTypes/Resource.h"
#include "UtopResourceInterface.h"
#include "Core/Player/UtopPlayerInterface.h"
#include "Plugin/IslandPluginInterface.h"
#include "UtopGameModeBase.generated.h"

class UUtopLayerGameActivatableWidget;
class AUtopSpawner;
class AUtopVillager;
class USoundControlBus;
class UCommonActivatableWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateVillagersSignature, int32, VillagerCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateResourcesSignature, EResourceType, Resource, int32, Value);

UCLASS()
class UTOPIA_API AUtopGameModeBase : public AGameModeBase ,public IUtopResourceInterface,
	public IUtopPlayerInterface, public IIslandPluginInterface
{
	GENERATED_BODY()

public:

	AUtopGameModeBase();

	/* UtopPlayerInterface */

	virtual void BeginBuild_Implementation(TSubclassOf<AUtopBaseInteractable> TargetClass, const TMap<EResourceType, int32>& InResourceCost) override;

	virtual void SwitchToBuildMode_Implementation(bool bSwitchToBuildMode) override;

	virtual void AddUI_Implementation(TSubclassOf<UCommonActivatableWidget> NewParam) override;

	virtual void RemoveCurretUI_Layer_Implementation();

	/* end UtopPlayerInterface*/

	/** IslandPluginInterface */
	virtual void ScaleUp_Implementation(float Delay) override;

	virtual void IslandGenComplete_Implementation() override;

	virtual void IslandSeed_Implementation(FRandomStream& Seed) override;

	virtual void SpawningComplete() override;

	/** end IslandPluginInterface */

	/* UtopResourceInterface */
	virtual void RemoveResource_Implementation(EResourceType& TargetResource, int32& Value) override;

	virtual void AddResource_Implementation(EResourceType TargetResource, int32 Value) override;

	virtual void GetCurrentResources_Implementation(TMap<EResourceType, int32>& InResources) override;

	virtual void RemoveTargetResources_Implementation(EResourceType TargetResource, int32 Value) override;

	virtual void CheckResource_Implementation(EResourceType TargetResource, bool& bCheck, int32& Num) override;

	/* end UtopResourceInterface */

	UPROPERTY(BlueprintAssignable)
	FOnUpdateVillagersSignature OnUpdateVillagersDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateResourcesSignature OnUpdateResourcesDelegate;


	//
	TMap<EResourceType, int32> Resources;

	int32 VillagerCount;

	UPROPERTY()
	TObjectPtr<UUtopLayerGameActivatableWidget> HUD;

	float StartTime;

	UPROPERTY()
	TObjectPtr<AUtopSpawner> SpawnRef;

	UPROPERTY()
	TObjectPtr<AActor> TownHall;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AActor> TownHallRef;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> VillagerRef;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTextureRenderTarget2D> GrassMoveTexture;

	void GetSpawnRef();

	void SpawnLoadedInteractables();

	void BeginAsyncSpawning();

	void BeginAsyncSpawningCompleted();

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundControlBus> NewMapBus;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnMarkerClass;

	UPROPERTY(EditAnywhere, Category = "Default")
	TSubclassOf<UUtopLayerGameActivatableWidget> LayerGameWidget;

	UFUNCTION(BlueprintCallable)
	void EndGame(bool bWin);

	UFUNCTION(BlueprintCallable)
	void SpawnVillagers(int32 Add);

	UFUNCTION(BlueprintCallable)
	void SpawnCharacters(int32 Add, TSubclassOf<AUtopBaseCharacter> CharacterClass, AActor* CharacterHouse, float Radius);

	void SpawnVillager();

	void SpawnCharacter(TSubclassOf<AUtopBaseCharacter> CharacterClass, AActor* CharacterHouse, float Radius);

	UPROPERTY(EditAnywhere, Category = "Default")
	int32 NumberOfVillagers = 0;

protected:

	virtual void BeginPlay() override;

private:

	void LoadVillagers();

	bool bGameEnded = false;

	void IslandGenDelayComleted();

	bool bIslandIsGenerated = false;
	
};
