#pragma once

#include "CoreMinimal.h"
#include "Save.generated.h"

class AUtopBaseInteractable;

USTRUCT(BlueprintType)
struct FSaveInteract
{
	GENERATED_BODY()

public:

	FSaveInteract(){}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform Location = FTransform();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AUtopBaseInteractable> Type = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Health = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Tag = FName();



};

USTRUCT(BlueprintType)
struct FVillager
{
	GENERATED_BODY()

public:

	FVillager() {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location = FVector();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Task = FName();



};


