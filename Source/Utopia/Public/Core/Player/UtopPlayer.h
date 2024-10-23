#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UtopPlayerInterface.h"
#include "UtopTypes/Input.h"
#include "UtopTypes/Resource.h"
#include "UtopPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UFloatingPawnMovement;
class UNiagaraComponent;
class AUtopPlayerController;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UNiagaraSystem;
class AUtopBaseInteractable;
class UMaterialParameterCollection;
class UUtopLayerGameActivatableWidget;
class UCommonActivatableWidget;

UCLASS()
class UTOPIA_API AUtopPlayer : public APawn, public IUtopPlayerInterface
{
	GENERATED_BODY()

public:
	
	AUtopPlayer();

	/* UtopPlayerInterface */

	virtual void BeginBuild_Implementation(TSubclassOf<AUtopBaseInteractable> TargetClass, const TMap<EResourceType, int32>& InResourceCost) override;

	virtual void SwitchToBuildMode_Implementation(bool bSwitchToBuildMode) override;

	virtual void AddUI_Implementation(TSubclassOf<UCommonActivatableWidget> NewParam) override;

	virtual void RemoveCurretUI_Layer_Implementation() override;

	/* end UtopPlayerInterface*/

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AUtopBaseInteractable> Spawn;

	UFUNCTION(BlueprintCallable)
	void DestroySpawn();

	UFUNCTION(BlueprintCallable)
	void RotateSpawn();

	UFUNCTION(BlueprintCallable)
	void SpawnBuildTarget();

	UFUNCTION(BlueprintCallable)
	void OverlapCheck(bool& bOverlapping);

	UFUNCTION(BlueprintCallable)
	void RemoveResources();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> TargetEffect;

	FTimerHandle UpdatePathTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInputType InputType = EInputType::EIT_Unknown;

	void UpdatePath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

private:

	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	bool VillagerOverlapCheck(AActor*& OverlappingActor);

	void VillagerSelect(AActor* InSelected);

	void ClosestHoverCheck();

	void VillagerRelease();

	void UpdateBuildAsset();

	bool CornersInNav();

	void CreateBuildOverlay();

	FTimerHandle ActorOverlapTimerHandle;

	void Dof();

	AUtopPlayerController* GetPlayerController();

	FVector2D ProjectMouseToGroundScreenPos();

	FVector ProjectMouseToGroundIntersection();

	bool ProjectMouseToGroundReturnValue();

	TObjectPtr<AUtopPlayerController> UtopPlayerController;

	FTimerHandle MoveTrackingTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Defaults")
	UMaterialParameterCollection* MaterialParamUtopia;

	/* Input */

	void VillagerStarted();

	void VillagerTriggered();

	void VillagerCanceled();

	void RemoveDragMoveContext();

	void VillagerCompleted();

	void BuildMoveTriggered();

	void BuildMoveCompleted();

	void Move(const FInputActionValue& InputActionValue);

	void Zoom(const FInputActionValue& InputActionValue);

	void Spin(const FInputActionValue& InputActionValue);

	void DragMove(const FInputActionValue& InputActionValue);


	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> BaseInputContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> VillagerModeContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DragMoveContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> BuildModeContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> VillagerInputAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> BuildMoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ZoomAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SpinAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DragMoveAction;

	/* Components */

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Cursor;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	TObjectPtr<UStaticMeshComponent> SpawnOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	TObjectPtr<UNiagaraComponent> PathEffect;

	/* Movement */

	void UpdateZoom();

	void MoveTarcking(float DeltaTime);

	FVector EdgeMoveDirection();

	float EdgeMoveStength();

	FVector CursorDistFromViewportCenterDirection(FVector2D A);

	void UpdateCursorPosition(float DeltaTime);

	UFUNCTION()
	void InputSwitch(EInputType NewInput);

	UPROPERTY(EditAnywhere, Category = "Movement")
	TObjectPtr<UCurveFloat> ZoomCurve;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ZoomValue = 0.5f;

	void TrackMove();

	/* Properties */

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> Selected;

	UPROPERTY(EditAnywhere)
	float StepRotation = 4.f;

	UPROPERTY(EditAnywhere)
	bool bCanDrop;

	UPROPERTY(EditAnywhere)
	FPostProcessSettings PostProcessSettings;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UObject> Target;

	UPROPERTY(EditAnywhere)
	float Aperture = 1.f;

	UPROPERTY(EditAnywhere)
	float SensorWidth = 24.576f;

	UPROPERTY(EditAnywhere)
	float FocalDistance;

	UPROPERTY(EditAnywhere)
	FTimerHandle HoverTimerHandle;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> HoverActor;

	UPROPERTY(EditAnywhere)
	FVector TargetHandle;

	UPROPERTY(EditAnywhere)
	bool bZoom;

	UPROPERTY(EditAnywhere)
	float ZoomTimeCheck;

	UPROPERTY(EditAnywhere)
	FVector2D TargetZooom;

	UPROPERTY(EditAnywhere)
	float ZoomDirection;

	UPROPERTY(EditAnywhere)
	FVector StoredMove;

	UPROPERTY(EditAnywhere)
	FTimerHandle ZoomHandle;

	UPROPERTY(EditAnywhere)
	FTimerHandle TrackHandle;

	UPROPERTY(EditAnywhere)
	float Limit = 10000.f;


	/* TODO: UI_LayerGame - Variable name UI_HUD */

	UPROPERTY(EditAnywhere)
	float FocusLength;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UObject> BuildMod;

	UPROPERTY(EditAnywhere)
	float ZoomCompare;

	UPROPERTY(EditAnywhere)
	FVector2D DragAxis;

	UPROPERTY(EditAnywhere)
	float EdgeMoveDistance = 50.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AUtopBaseInteractable> TargetSpawnClass;

	//UPROPERTY(EditAnywhere)
	//TObjectPtr<UMaterialInstanceDynamic> PlaceableMaterial;

	UPROPERTY(EditAnywhere)
	TArray<FVector> PathPoints;

	UPROPERTY(EditAnywhere)
	bool bBlockDrag;

	UPROPERTY(EditAnywhere)
	bool bMoveSpawnn;

	UPROPERTY(EditAnywhere)
	TMap<EResourceType, int32> ResourceCost;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> VillagerTarget;

	/* TODO: Check name of the variable */
	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> Hover_Actor;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> VillagerAction;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	TObjectPtr<UStaticMesh> PlaceableMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	TObjectPtr<UMaterialInterface> PlaceableMaterial;

	float ZoomCurveValue;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	float ZoomThreshold = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	float ZoomInterpSpeed = 2.f;

	float CacheZoomCurveValue;

	float CacheZoomValue;

	bool bZoomIsNormalized = true;

	bool bUnlockNormalizedProcess = true;



	void NormalizeUpdateZoom(float DeltaTime);



};
