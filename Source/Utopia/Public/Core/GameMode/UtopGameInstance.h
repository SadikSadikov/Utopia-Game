// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Core/Save/GameInstanceInterface.h"
#include "Plugin/IslandPluginInterface.h"
#include "UtopGameInstance.generated.h"

class UAudioComponent;
class UUserWidget;
class UUtopSaveGame;
class UBaseTransitionUserWidget;
class USoundControlBus;

UCLASS()
class UTOPIA_API UUtopGameInstance : public UGameInstance, public IGameInstanceInterface, public IIslandPluginInterface
{
	GENERATED_BODY()

public:

	UUtopGameInstance();

	virtual void Init() override;

	/** IslandPluginInterface */

	virtual void ScaleUp_Implementation(float Delay) override;

	virtual void IslandGenComplete_Implementation() override;

	virtual void IslandSeed_Implementation(FRandomStream& Seed) override;

	virtual void SpawningComplete() override;

	/** end IslandPluginInterface */

	/** GameInstanceInterface */

	virtual bool CheckSaveBool_Implementation() override;

	virtual FRandomStream GetSeed() override;

	virtual TArray<FSaveInteract> GetAllInteractables() override;

	virtual UUtopSaveGame* GetSave() override;

	virtual void StartNewLevel() override;

	virtual void SaveGame() override;

	virtual void UpdateSaveData() override;

	virtual void UpdateAllInteractables_Implementation() override;

	virtual void UpdateAllResource_Implementation(const TMap<EResourceType, int32>& InResources) override;

	virtual void UpdateAllVillagers_Implementation() override;

	virtual void ClearSave_Implementation(bool bClearSeed) override;

	virtual void LoadLevel() override;

	/** end  GameInstanceInterface */

	UFUNCTION(BlueprintCallable)
	void PlayMusic(USoundBase* Audio, float Volume, bool Persist);

	UPROPERTY(EditAnywhere, Category = "Music")
	TObjectPtr<USoundControlBus> WinLoseMusic;

	UPROPERTY(EditAnywhere, Category = "Music")
	TObjectPtr<USoundControlBus> StopGameMusic;

	UPROPERTY(EditAnywhere, Category = "Music")
	TObjectPtr<USoundControlBus> PianoMusic;

	UFUNCTION(BlueprintCallable)
	void StopMusic();

	void LoadGame();

	UFUNCTION(BlueprintCallable)
	void OpenLevel(TSoftObjectPtr<UWorld> InLevel);

	void LoadingUI();

	void ClearSaveData();

	void OnSaveCompleted(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	UPROPERTY()
	TSoftObjectPtr<UWorld> Level;

	UFUNCTION(BlueprintCallable)
	void TransitionIn();

	UFUNCTION(BlueprintCallable)
	void TransitionOut();

	UFUNCTION()
	void OnOpenLevelDelay();
	
	// Components
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UAudioComponent> AudioComponent;

	//
	int32 TargetSeed = 444;

	UPROPERTY()
	TObjectPtr<UBaseTransitionUserWidget> TransitionWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBaseTransitionUserWidget> TransitionWidgetClass;

	bool bHasSave;

	UPROPERTY()
	TObjectPtr<UUtopSaveGame> SaveRef;
	
	float StartGameOffset;

	bool bMusicPlaying;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<float> SoundMixes;

	UPROPERTY(EditDefaultsOnly, Category = "Default")
	TSubclassOf<UUtopSaveGame> SaveGameClass;

	FORCEINLINE const TMap<EResourceType, int32>& GetInitialResources() { return InitialResources; }

private:

	UPROPERTY(EditDefaultsOnly)
	TMap<EResourceType, int32> InitialResources;
};
