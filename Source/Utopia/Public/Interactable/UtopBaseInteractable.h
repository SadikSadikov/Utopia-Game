// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UtopBaseInteractable.generated.h"

class UBoxComponent;
class UTimelineComponent;

UCLASS()
class UTOPIA_API AUtopBaseInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	AUtopBaseInteractable();


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "Placement Mode"))
	void ReceivePlacementMode();

	virtual void PlacementMode();

	UFUNCTION(BlueprintCallable)
	void PlayWobble(const FVector& InVec);

	UFUNCTION(BlueprintCallable)
	void EndWobble();

	UFUNCTION()
	void HandleWobbleProgress(float Value);

	// Progression
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Progression")
	float ProgressionState;

	// Progression

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Progression")
	bool bRequireBuild;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build", meta = (DisplayName = "SetProgressionsState"))
	void ReceiveSetProgressionsState(float Progression);

	virtual void SetProgressionsState(float Progression);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "Interact"))
	void ReceiveInteract(float& InCollectionTime);

	virtual void Interact(float& Delay);


protected:

	virtual void BeginPlay() override;


	// Ground Blend

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GroundBlend")
	TObjectPtr<UTextureRenderTarget2D> RenderTargetDraw;

	UPROPERTY(EditDefaultsOnly, Category = "GroundBlend")
	UMaterialInterface* RenderMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GroundBlend")
	float OutlineDraw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GroundBlend")
	bool bEnableGroundBlend = true;

	// Progression

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Progression")
	TArray<UStaticMesh*> MeshList;

	// SpawnInfo

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnInfo")
	float BoundGap;

	// Others

	UPROPERTY(EditDefaultsOnly, Category = "Timeline|Curve")
	TObjectPtr<UCurveFloat> TimelineCurve;


	UPROPERTY(EditDefaultsOnly, Category = "Timeline|Curve")
	TObjectPtr<UCurveFloat> WobbleCurve;

	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTimelineComponent* TimelineComponent;





private:

	FVector2D TransformToTextureScreenPosition(const FVector2D& InVec);

	FVector2D TransformToTextureScreenSize(const FVector2D& InVec);

	// Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;





public:

	FORCEINLINE const UBoxComponent* GetBoxComponent() { return Box; }

	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() { return Mesh; }
};
