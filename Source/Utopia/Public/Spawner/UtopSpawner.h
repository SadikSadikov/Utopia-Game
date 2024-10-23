// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utopia/Public/UtopTypes/Spawn.h"
#include "UtopSpawner.generated.h"

class ANavigationData;

UCLASS()
class UTOPIA_API AUtopSpawner : public AActor
{
	GENERATED_BODY()
	
public:	

	AUtopSpawner();

	virtual void Tick(float DeltaTime) override;

	void SpawnMeshOnly();

	void SpawnRandom();

	//Class Load

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClassLoad")
	int32 ClassRefIndex;

	FTimerHandle AsyncTimerHandle;

	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	float Radius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	FRandomStream Seed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	FTimerHandle NavCheckHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	TArray<FSpawnData> SpawnTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	int32 IndexCounter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<ANavigationData> NavData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	int32 Counter = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	float TotalCount = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	TArray<FSpawnInstance> SpawnInstances;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	bool bActorSwitch = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	bool bAutoSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	bool bCallSave = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	bool bAsyncComplete = false;

protected:

	virtual void BeginPlay() override;


private:

	void AsyncLaodClasses();

	void AsyncClass();

	void AsyncLoadClassAssets();

	void BuildNavSystemAndLoadSoftClasses();

	void OnReadyToSpawn();

	void SpawnAssets(TSubclassOf<AActor> Class, FSpawnData SpawnParams);

	void SpawnInst(UInstancedStaticMeshComponent* Class, float InRadius, int32 BiomeCount, int32 MaxSpawn);

	void FinishedSpawning();


};
