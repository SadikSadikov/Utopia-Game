#pragma once

#include "CoreMinimal.h"
#include "Spawn.generated.h"

USTRUCT(BlueprintType)
struct FSpawnData
{
	GENERATED_BODY()

public:

	FSpawnData(){}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<AActor> ClassRef = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BiomeScale = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BiomeCount = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 SpawnPerBiome = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RandomRotationRange = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScaleRange = 0.f;
};

USTRUCT(BlueprintType)
struct FSpawnInstance
{
	GENERATED_BODY()

public:

	FSpawnInstance() {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UStaticMesh> ClassRef = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BiomeScale = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BiomeCount = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 SpawnPerBiome = 0;

};
