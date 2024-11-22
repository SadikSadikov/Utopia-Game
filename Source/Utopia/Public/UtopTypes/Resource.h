#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Resource.generated.h"

class AUtopBaseInteractable;

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	ERT_None UMETA(DisplayName = "None"),
	ERT_Food UMETA(DisplayName = "Food"),
	ERT_Wood UMETA(DisplayName = "Wood"),
	ERT_Stone UMETA(DisplayName = "Stone"),

	ERT_MAXIMUM UMETA(DisplayName = "MAXIMUM")

};

USTRUCT(BlueprintType)
struct FResource : public FTableRowBase
{
	GENERATED_BODY()

public:

	FResource() {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Title = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<AUtopBaseInteractable> TargetClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EResourceType, int32> Cost = TMap<EResourceType, int32>();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FColor TabColor = FColor::Blue;

};
