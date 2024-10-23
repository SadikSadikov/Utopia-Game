// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Player/UtopPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Player/UtopPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NavigationSystem.h"
#include "NavigationSystem/Public/NavigationPath.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Villagers/UtopVillagerInterface.h"
#include "EnhancedInputComponent.h"
#include "Interactable/UtopBaseInteractable.h"
#include "Core/Player/UtopShareFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Core/GameMode/UtopFunctionLibrary.h"
#include "Utopia/PrintStrings.h"
#include "Core/Save/GameInstanceInterface.h"
#include "GameFramework/GameModeBase.h"
#include "Core/GameMode/UtopResourceInterface.h"

AUtopPlayer::AUtopPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	SetRootComponent(DefaultSceneRoot);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 1100.f;
	CameraBoom->SetRelativeRotation(FRotator(-40.f, 0.f, 0.f));
	CameraBoom->SocketOffset = FVector(-300.f, 0.f, 80);
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->SetFieldOfView(20.f);

	Cursor = CreateDefaultSubobject<UStaticMeshComponent>("Cursor");
	Cursor->SetupAttachment(GetRootComponent());
	Cursor->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	Cursor->SetRelativeScale3D(FVector(2.f, 2.f, 1.f));
	Cursor->SetGenerateOverlapEvents(false);
	Cursor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Cursor->bNeverDistanceCull = true;
	Cursor->bAllowCullDistanceVolume = false;

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetupAttachment(GetRootComponent());
	Collision->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	Collision->SetSphereRadius(130.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	Collision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);


	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("FloatingPawnMovement");
	FloatingPawnMovement->MaxSpeed = 1500.f;
	FloatingPawnMovement->Acceleration = 8000.f;
	FloatingPawnMovement->Deceleration = 6500.f;
	FloatingPawnMovement->SetPlaneConstraintNormal(FVector(0.f, 0.f, 1.f));
	FloatingPawnMovement->bConstrainToPlane = true;
	FloatingPawnMovement->bSnapToPlaneAtStart = true;


}

void AUtopPlayer::DestroySpawn()
{
	if (Spawn && SpawnOverlay)
	{
		Spawn->Destroy();

		SpawnOverlay->DestroyComponent();
	}
}

void AUtopPlayer::RotateSpawn()
{
	if (Spawn)
	{
		Spawn->SetActorRotation(Spawn->GetActorRotation() + FRotator(0.f, 90.f, 0.f));
	}
}

void AUtopPlayer::SpawnBuildTarget()
{
	if (bCanDrop && Spawn)
	{
		AUtopBaseInteractable* Interactable = GetWorld()->SpawnActor<AUtopBaseInteractable>(TargetSpawnClass, Spawn->GetActorTransform());

		if (Interactable)
		{
			Interactable->SetProgressionsState(0.f);

			RemoveResources();

			if (UGameplayStatics::GetGameInstance(this)->Implements<UGameInstanceInterface>())
			{
				IGameInstanceInterface::Execute_UpdateAllInteractables(UGameplayStatics::GetGameInstance(this));

				UpdateBuildAsset();
			}
		}
	}
}

void AUtopPlayer::OverlapCheck(bool& bOverlapping)
{
	TArray<AActor*> OverlappingActors;
	Collision->GetOverlappingActors(OverlappingActors, AUtopBaseInteractable::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		bOverlapping = OverlappingActors.Contains(Spawn);
	}
}

void AUtopPlayer::RemoveResources()
{
	TArray<EResourceType> Keys;
	ResourceCost.GetKeys(Keys);

	for (const EResourceType& Key : Keys)
	{
		if (UGameplayStatics::GetGameMode(this)->Implements<UUtopResourceInterface>())
		{
			IUtopResourceInterface::Execute_RemoveTargetResources(UGameplayStatics::GetGameMode(this), Key,
				*ResourceCost.Find(Key));
		}
	}

	if (UGameplayStatics::GetGameMode(this)->Implements<UUtopResourceInterface>())
	{
		TMap<EResourceType, int32> InResources;
		IUtopResourceInterface::Execute_GetCurrentResources(UGameplayStatics::GetGameMode(this), InResources);

		TArray<EResourceType> UpdatedKeys;
		ResourceCost.GetKeys(UpdatedKeys);

		for (const EResourceType& Key : UpdatedKeys)
		{
			if (InResources.Find(Key) && ResourceCost.Find(Key))
			{
				bool bCheck = *InResources.Find(Key) >= *ResourceCost.Find(Key);
				if (!bCheck)
				{
					if (UGameplayStatics::GetGameMode(this)->Implements<UUtopPlayerInterface>())
					{
						IUtopPlayerInterface::Execute_RemoveCurretUI_Layer(UGameplayStatics::GetGameMode(this));

						DestroySpawn();

						break;
					}
				}
			}
			
		}
	}

}

void AUtopPlayer::BeginPlay()
{
	Super::BeginPlay();

	UpdateZoom();

	//GetWorld()->GetTimerManager().SetTimer(MoveTrackingTimerHandle, this, &AUtopPlayer::MoveTarcking, 0.016667, true, 0.f);

	if (GetPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetPlayerController()->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(BaseInputContext, 0);
			Subsystem->AddMappingContext(VillagerModeContext, 0);
		}
	}

	OnActorBeginOverlap.AddDynamic(this, &AUtopPlayer::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AUtopPlayer::ActorEndOverlap);

}

void AUtopPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(VillagerInputAction, ETriggerEvent::Triggered, this, &AUtopPlayer::VillagerTriggered);
		EnhancedInputComponent->BindAction(VillagerInputAction, ETriggerEvent::Started, this, &AUtopPlayer::VillagerStarted);
		EnhancedInputComponent->BindAction(VillagerInputAction, ETriggerEvent::Canceled, this, &AUtopPlayer::VillagerCanceled);
		EnhancedInputComponent->BindAction(VillagerInputAction, ETriggerEvent::Completed, this, &AUtopPlayer::VillagerCompleted);

		EnhancedInputComponent->BindAction(BuildMoveAction, ETriggerEvent::Triggered, this, &AUtopPlayer::BuildMoveTriggered);
		EnhancedInputComponent->BindAction(BuildMoveAction, ETriggerEvent::Completed, this, &AUtopPlayer::BuildMoveCompleted);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUtopPlayer::Move);

		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AUtopPlayer::Zoom);

		EnhancedInputComponent->BindAction(SpinAction, ETriggerEvent::Triggered, this, &AUtopPlayer::Spin);

		EnhancedInputComponent->BindAction(DragMoveAction, ETriggerEvent::Triggered, this, &AUtopPlayer::DragMove);
	}

}


void AUtopPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AUtopPlayerController* UtopPC = Cast<AUtopPlayerController>(NewController);

	if (UtopPC)
	{
		UtopPC->KeySwitchDelegate.AddDynamic(this, &AUtopPlayer::InputSwitch);
	}

}

void AUtopPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTarcking(DeltaTime);

	NormalizeUpdateZoom(DeltaTime);

}

void AUtopPlayer::UpdateZoom()
{

	if (bZoomIsNormalized)
	{
		ZoomValue = FMath::Clamp((ZoomDirection * 0.01f) + ZoomValue, 0.f, 1.f);

		ZoomCurveValue = ZoomCurve.Get()->FloatCurve.Eval(ZoomValue);

		CameraBoom->TargetArmLength = FMath::Lerp(8000.f, 40000.f, ZoomCurveValue);

		CameraBoom->SetRelativeRotation(FRotator(FMath::Lerp(-40.f, -55.f, ZoomCurveValue), 0.f, 0.f));

		FloatingPawnMovement->MaxSpeed = FMath::Lerp(1000.f, 6000.f, ZoomCurveValue);

		Dof();

		FollowCamera->SetFieldOfView(FMath::Lerp(20.f, 15.f, ZoomCurveValue));

		if (CameraBoom->TargetArmLength < ZoomThreshold)
		{
			bZoomIsNormalized = false;

		}
		else
		{
			CacheZoomCurveValue = ZoomCurveValue;
			CacheZoomValue = ZoomValue;
		}
	}

	

}

void AUtopPlayer::NormalizeUpdateZoom(float DeltaTime)
{
	if (!bZoomIsNormalized)
	{
		
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, ZoomThreshold + 100.f, DeltaTime, ZoomInterpSpeed);

		CameraBoom->SetRelativeRotation(FMath::RInterpTo(CameraBoom->GetRelativeRotation(),
			FRotator(FMath::Lerp(-40.f, -55.f, CacheZoomCurveValue), 0.f, 0.f), DeltaTime, ZoomInterpSpeed));

		FloatingPawnMovement->MaxSpeed = FMath::FInterpTo(FloatingPawnMovement->MaxSpeed, FMath::Lerp(1000.f, 6000.f, CacheZoomCurveValue),
			DeltaTime, ZoomInterpSpeed);

		Dof();

		FollowCamera->SetFieldOfView(FMath::FInterpTo(FollowCamera->FieldOfView, FMath::Lerp(20.f, 15.f, CacheZoomCurveValue),
			DeltaTime, ZoomInterpSpeed));

		if (CameraBoom->TargetArmLength >= ZoomThreshold)
		{
			bZoomIsNormalized = true;

			ZoomCurveValue = CacheZoomCurveValue;
			ZoomValue = CacheZoomValue;
		}
	}
}

void AUtopPlayer::Dof()
{
	FPostProcessSettings Settings;

	Settings.bOverride_DepthOfFieldFstop = true;
	Settings.bOverride_DepthOfFieldSensorWidth = true;
	Settings.bOverride_DepthOfFieldFocalDistance = true;

	Settings.DepthOfFieldFstop = 1.2f;
	Settings.DepthOfFieldSensorWidth = 250.f;
	Settings.DepthOfFieldFocalDistance = CameraBoom->TargetArmLength < ZoomThreshold ? 50000.f : CameraBoom->TargetArmLength;

	FollowCamera->PostProcessSettings = Settings;
}

void AUtopPlayer::MoveTarcking(float DeltaTime)
{

	FVector NormalizedActorLocation = GetActorLocation().GetSafeNormal(0.0001f);
	float CalcScaleValue = UKismetMathLibrary::Max((GetActorLocation().Length() - 9000.f) / 5000.f, 0.f);
	FVector CalcWorldDirection = FVector(NormalizedActorLocation.X, NormalizedActorLocation.Y, 0.f) * -1.f;

	// Keep player within playspace
	AddMovementInput(CalcWorldDirection, CalcScaleValue);

	UpdateCursorPosition(DeltaTime);

	// Edge of screen movement
	AddMovementInput(EdgeMoveDirection(), EdgeMoveStength());

	Collision->SetWorldLocation(ProjectMouseToGroundIntersection() + FVector(0.f, 0.f, 10.f));


}

void AUtopPlayer::UpdateCursorPosition(float DeltaTime)
{
	if (InputType != EInputType::EIT_Unknown)
	{
		FTransform TargetTransform;

		if (HoverActor)
		{
			FVector Origin;
			FVector BoxExtend;
			HoverActor->GetActorBounds(true, Origin, BoxExtend);

			FVector2D ScaleValue2D = FVector2D(BoxExtend.X, BoxExtend.Y);
			float ScaleAbsMaxValue = UKismetMathLibrary::GetAbsMax2D(ScaleValue2D);
			float ScaleValue = (ScaleAbsMaxValue / 50.f) + (UKismetMathLibrary::Sin(UKismetSystemLibrary::GetGameTimeInSeconds(this) * 5.f) * 0.25f) + 1.f;
			TargetTransform = FTransform(FRotator::ZeroRotator, FVector(Origin.X, Origin.Y, 20.f), FVector(ScaleValue, ScaleValue, 1.f));
		}
		else
		{
			TargetTransform = FTransform(Collision->GetComponentRotation(), Collision->GetComponentLocation(), FVector(2.f, 2.f, 1.f));
		}

		float DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(this);
		FTransform InterpTransform = UKismetMathLibrary::TInterpTo(Cursor->GetComponentTransform(), TargetTransform, DeltaSeconds, 20.f);
		Cursor->SetWorldTransform(InterpTransform);

	}
}

FVector AUtopPlayer::ProjectMouseToGroundIntersection()
{
	int32 ViewportSizeX;
	int32 ViewportSizeY;
	GetPlayerController()->GetViewportSize(ViewportSizeX, ViewportSizeY);
	FVector2D LockProjectionToCenter(ViewportSizeX / 2.f, ViewportSizeY / 2.f);

	float MouseViewportPosX;
	float MouseViewportPosY;
	bool bMouseClick = GetPlayerController()->GetMousePosition(MouseViewportPosX, MouseViewportPosY);
	FVector2D MouseViewportPos(MouseViewportPosX, MouseViewportPosY);

	FVector2D SelectBasedOnInput;

	if (InputType != EInputType::EIT_KeyMouse)
	{
		SelectBasedOnInput = LockProjectionToCenter;
	}
	else 
	{
		if (bMouseClick)
		{
			SelectBasedOnInput = MouseViewportPos;
		}
		else
		{
			SelectBasedOnInput = LockProjectionToCenter;
		}
	}


	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (PlayerController)
	{
		FVector WorldPosition;
		FVector WorldDirection;
		UGameplayStatics::DeprojectScreenToWorld(PlayerController, SelectBasedOnInput, WorldPosition, WorldDirection);

		float T;
		FVector Intersection;

		FPlane Plane = UKismetMathLibrary::MakePlaneFromPointAndNormal(FVector::Zero(), FVector(0.f, 0.f, 1.f));

		UKismetMathLibrary::LinePlaneIntersection(WorldPosition, WorldPosition + (WorldDirection * 100000.0), Plane, T, Intersection);

		
		return Intersection;
	}

	return FVector();
}

void AUtopPlayer::InputSwitch(EInputType NewInput)
{
	InputType = NewInput;

	if (InputType == EInputType::EIT_Gamepad)
	{
		Collision->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	}

}

void AUtopPlayer::TrackMove()
{
	if (ProjectMouseToGroundReturnValue())
	{
		FVector CameraBoomSocketOffset = CameraBoom->SocketOffset;

		FVector BoomForwardOffset = CameraBoom->GetForwardVector() * (CameraBoom->TargetArmLength - CameraBoomSocketOffset.X) * -1.f;
		FVector CameraBoomWorldLocation = BoomForwardOffset + CameraBoom->GetUpVector() * CameraBoomSocketOffset.Z + CameraBoom->GetComponentLocation();
		FVector RelativeCameraOffset = CameraBoomWorldLocation - FollowCamera->GetComponentLocation();

		StoredMove = TargetHandle - ProjectMouseToGroundIntersection() - RelativeCameraOffset;

		AddActorWorldOffset(FVector(StoredMove.X, StoredMove.Y, 0.f));

	}
}


FVector AUtopPlayer::EdgeMoveDirection()
{

	int32 ViewportSizeX;
	int32 ViewportSizeY;
	GetPlayerController()->GetViewportSize(ViewportSizeX, ViewportSizeY);
	FVector2D ViewportCenter = FVector2D(ViewportSizeX, ViewportSizeY) / 2.f;

	FVector2D CalcViewportCenterAndProjectMouseTouch = ProjectMouseToGroundScreenPos() - ViewportCenter;

	FVector Direction = UKismetMathLibrary::TransformDirection(GetActorTransform(),
		CursorDistFromViewportCenterDirection(CalcViewportCenterAndProjectMouseTouch));

	return Direction;
}

float AUtopPlayer::EdgeMoveStength()
{
	return 1.f;
}

FVector2D AUtopPlayer::ProjectMouseToGroundScreenPos()
{
	int32 ViewportSizeX;
	int32 ViewportSizeY;
	GetPlayerController()->GetViewportSize(ViewportSizeX, ViewportSizeY);
	FVector2D LockProjectionToCenter = FVector2D(ViewportSizeX, ViewportSizeY) / 2.f;


	float MouseViewportPosX;
	float MouseViewportPosY;
	bool bMouseClick = GetPlayerController()->GetMousePosition(MouseViewportPosX, MouseViewportPosY);
	FVector2D MouseViewportPos(MouseViewportPosX, MouseViewportPosY);

	FVector2D SelectBasedOnInput;

	if (InputType != EInputType::EIT_KeyMouse)
	{
		SelectBasedOnInput = LockProjectionToCenter;
	}
	else
	{
		if (bMouseClick)
		{
			SelectBasedOnInput = MouseViewportPos;
		}
		else
		{
			SelectBasedOnInput = LockProjectionToCenter;
		}
	}

	return SelectBasedOnInput;
}

bool AUtopPlayer::ProjectMouseToGroundReturnValue()
{
	float MouseViewportPosX;
	float MouseViewportPosY;
	bool bMouseClick = GetPlayerController()->GetMousePosition(MouseViewportPosX, MouseViewportPosY);
	FVector2D MouseViewportPos(MouseViewportPosX, MouseViewportPosY);

	bool bInputType = false;
	switch (InputType)
	{

	case EInputType::EIT_Unknown:
		bInputType = false;
		break;

	case EInputType::EIT_KeyMouse:
		bInputType = bMouseClick;
		break;

	case EInputType::EIT_Gamepad:
		bInputType = true;
		break;

	}


	return bInputType;
}


FVector AUtopPlayer::CursorDistFromViewportCenterDirection(FVector2D A)
{
	float A_X = UKismetMathLibrary::SignOfFloat(A.X);
	float A_Y = UKismetMathLibrary::SignOfFloat(A.Y);


	int32 ViewportSizeX;
	int32 ViewportSizeY;
	GetPlayerController()->GetViewportSize(ViewportSizeX, ViewportSizeY);

	float InputTypeValue = 0.f;
	switch (InputType)
	{

	case EInputType::EIT_Unknown:
		InputTypeValue = 0.f;
		break;

	case EInputType::EIT_KeyMouse:
		InputTypeValue = 1.f;
		break;

	case EInputType::EIT_Gamepad:
		InputTypeValue = 2.f;
		break;
	}

	float CalcViewportSizeX = (ViewportSizeX / 2.f) - (EdgeMoveDistance * InputTypeValue);
	float CalcViewportSizeY = (ViewportSizeY / 2.f) - (EdgeMoveDistance * InputTypeValue);

	float OffsetTrackedMouseX = UKismetMathLibrary::Max(FMath::Abs(A.X) - CalcViewportSizeX, 0.f) / EdgeMoveDistance;
	float OffsetTrackedMouseY = UKismetMathLibrary::Max(FMath::Abs(A.Y) - CalcViewportSizeY, 0.f) / EdgeMoveDistance;


	float Result_X = A_Y * OffsetTrackedMouseY * -1.f;
	float Result_Y = A_X * OffsetTrackedMouseX;

	return FVector(Result_X, Result_Y, 0.f);
}

void AUtopPlayer::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!IsValid(HoverActor))
	{
		HoverActor = OtherActor;
		GetWorld()->GetTimerManager().SetTimer(ActorOverlapTimerHandle, this, &AUtopPlayer::ClosestHoverCheck, 0.01f, true);
	}
}

void AUtopPlayer::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	TArray<AActor*> OverlappingActors;

	GetOverlappingActors(OverlappingActors, AActor::StaticClass());
	if (OverlappingActors.IsEmpty())
	{
		UKismetSystemLibrary::DelayUntilNextTick(this, FLatentActionInfo());
		HoverActor = nullptr;
	}
}

void AUtopPlayer::ClosestHoverCheck()
{

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AActor::StaticClass());

	if (OverlappingActors.Num() == 0)
	{
		GetWorld()->GetTimerManager().PauseTimer(ActorOverlapTimerHandle);
	}
	else
	{
		AActor* NewHover = nullptr;
		TArray<AActor*> OverlappingPawns;
		GetOverlappingActors(OverlappingPawns, AActor::StaticClass());

		int32 Index = 0;
		for (AActor* Pawn : OverlappingPawns)
		{
			if (Index == 0)
			{
				NewHover = Pawn;
				continue;
			}

			float FirstDistance = UKismetMathLibrary::Vector_Distance(Pawn->GetActorLocation(), Collision->GetComponentLocation());
			float SecondDistance = UKismetMathLibrary::Vector_Distance(Collision->GetComponentLocation(), NewHover->GetActorLocation());
			if (FirstDistance < SecondDistance)
			{
				NewHover = Pawn;
			}
			Index++;
		}

		if (HoverActor != NewHover)
		{
			HoverActor = NewHover;
		}

	}
}

bool AUtopPlayer::VillagerOverlapCheck(AActor*& OverlappingActor)
{
	TArray<AActor*> OverlappingActors;

	GetOverlappingActors(OverlappingActors, APawn::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		if (IsValid(OverlappingActors[0]))
		{
			OverlappingActor = OverlappingActors[0];
			return true;
		}
	}
	OverlappingActor = nullptr;
	return false;
}

void AUtopPlayer::VillagerSelect(AActor* InSelected)
{
	Selected = InSelected;
	if (TargetEffect)
	{
		PathEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(TargetEffect, DefaultSceneRoot, FName(""), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);

		GetWorld()->GetTimerManager().SetTimer(UpdatePathTimerHandle, this, &AUtopPlayer::UpdatePath, 0.01f, true);
	}

}

void AUtopPlayer::VillagerRelease()
{
	GetWorld()->GetTimerManager().PauseTimer(UpdatePathTimerHandle);
	if (PathEffect)
	{
		PathEffect->DestroyComponent();
	}
	Selected = nullptr;

}

void AUtopPlayer::UpdatePath()
{
	UNavigationPath* NavPathPoints = UNavigationSystemV1::FindPathToLocationSynchronously(this, Collision->GetComponentLocation(), Selected->GetActorLocation());

	if (NavPathPoints->PathPoints.Num() > 0)
	{
		PathPoints = NavPathPoints->PathPoints;

		PathPoints[0] = Collision->GetComponentLocation();
		PathPoints[PathPoints.Num() - 1] = Selected->GetActorLocation();

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(PathEffect, FName("TargetPath"), PathPoints);

	}
}

AUtopPlayerController* AUtopPlayer::GetPlayerController()
{
	if (UtopPlayerController == nullptr)
	{
		UtopPlayerController = Cast<AUtopPlayerController>(GetController());
	}

	return UtopPlayerController;
}

void AUtopPlayer::RemoveDragMoveContext()
{
	if (GetPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetPlayerController()->GetLocalPlayer()))
		{
			FModifyContextOptions ContextOptions;
			ContextOptions.bIgnoreAllPressedKeysUntilRelease = true;
			ContextOptions.bForceImmediately = true;
			Subsystem->RemoveMappingContext(DragMoveContext, ContextOptions);
		}
	}
}


void AUtopPlayer::BeginBuild_Implementation(TSubclassOf<AUtopBaseInteractable> TargetClass, const TMap<EResourceType, int32>& InResourceCost)
{
	TargetSpawnClass = TargetClass;
	ResourceCost = InResourceCost;

	if (IsValid(Spawn))
	{
		Spawn->Destroy();
	}

	Spawn = GetWorld()->SpawnActor<AUtopBaseInteractable>(TargetSpawnClass, FTransform(FRotator::ZeroRotator, GetActorLocation(), FVector::OneVector));

	Spawn->PlacementMode();
	CreateBuildOverlay();
}

void AUtopPlayer::SwitchToBuildMode_Implementation(bool bSwitchToBuildMode)
{
	if (GetPlayerController() && VillagerModeContext && BuildModeContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetPlayerController()->GetLocalPlayer()))
		{
			if (bSwitchToBuildMode)
			{
				Subsystem->RemoveMappingContext(VillagerModeContext);
				Subsystem->AddMappingContext(BuildModeContext, 0);
			}
			else
			{
				Subsystem->RemoveMappingContext(BuildModeContext);
				Subsystem->AddMappingContext(VillagerModeContext, 0);
			}

		}

	}
}

void AUtopPlayer::AddUI_Implementation(TSubclassOf<UCommonActivatableWidget> NewParam)
{
}

void AUtopPlayer::RemoveCurretUI_Layer_Implementation()
{
}


void AUtopPlayer::CreateBuildOverlay()
{
	if (PlaceableMesh && PlaceableMaterial)
	{
		FVector Origin;
		FVector BoxExtent;
		Spawn->GetActorBounds(false, Origin, BoxExtent, false);

		UStaticMeshComponent* SpawnedMesh = NewObject<UStaticMeshComponent>(this);
		SpawnedMesh->SetStaticMesh(PlaceableMesh);
		SpawnedMesh->SetMaterial(0, PlaceableMaterial);
		SpawnedMesh->SetGenerateOverlapEvents(false);
		SpawnedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		FTransform SpawnedMeshTransfrom(FRotator::ZeroRotator, FVector::ZeroVector, BoxExtent / 100.f);
		SpawnedMesh->SetRelativeTransform(SpawnedMeshTransfrom);
		SpawnedMesh->RegisterComponent();

		if (SpawnedMesh)
		{
			SpawnOverlay = SpawnedMesh;

			FAttachmentTransformRules TransfromRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
			SpawnOverlay->AttachToComponent(Spawn->GetStaticMeshComponent(), TransfromRules);
		}

		UpdateBuildAsset();

	}
}


void AUtopPlayer::UpdateBuildAsset()
{
	if (ProjectMouseToGroundReturnValue() && Spawn)
	{
		FVector NewLocation = UKismetMathLibrary::VInterpTo(Spawn->GetActorLocation(),
			UUtopShareFunctionLibrary::ConvertToSteppedPos(ProjectMouseToGroundIntersection()), UGameplayStatics::GetWorldDeltaSeconds(this), 10.f);
		Spawn->SetActorLocation(NewLocation);

		TArray<AActor*> OverlappingActors;
		Spawn->GetOverlappingActors(OverlappingActors, AUtopBaseInteractable::StaticClass());
		if (OverlappingActors.Num() == 0)
		{
			bCanDrop = CornersInNav();

		}
		else
		{
			bCanDrop = false;
		}

		if (MaterialParamUtopia)
		{
			FLinearColor LinearColor;
			LinearColor.R = NewLocation.X;
			LinearColor.G = NewLocation.Y;
			LinearColor.B = NewLocation.Z;
			LinearColor.A = bCanDrop ? 1.0f : 0.0f;
			UKismetMaterialLibrary::SetVectorParameterValue(this, MaterialParamUtopia, FName("TargetPosition"), LinearColor);
		}
	

	}
}

bool AUtopPlayer::CornersInNav()
{
	bool bOnIsland = false;

	FVector Origin;
	FVector BoxExtent;
	float SphereRadius;
	
	UKismetSystemLibrary::GetComponentBounds(Spawn->GetBoxComponent(), Origin, BoxExtent, SphereRadius);
	FVector ComponentBounds = Origin + FVector(BoxExtent.X * 1.05f, BoxExtent.Y * 1.05f, 0);

	FVector Start(ComponentBounds.X, ComponentBounds.Y, 100.f);
	FVector End(ComponentBounds.X, ComponentBounds.Y, -1.f);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Spawn);

	// FCollisionShape::LineShape

	bool bHitFirst = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeBox(FVector(50.f)),
		QueryParams );

	if (!bHitFirst) return bOnIsland;


	ComponentBounds = Origin + FVector((BoxExtent.X * 1.05f) * -1.f, BoxExtent.Y * 1.05f, 0);

	bool bHitSecond = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeBox(FVector(50.f)),
		QueryParams);

	if (!bHitSecond) return bOnIsland;


	ComponentBounds = Origin + FVector(BoxExtent.Y * 1.05f, (BoxExtent.Y * 1.05f) * -1.f, 0);

	bool bHitThirth = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeBox(FVector(50.f)),
		QueryParams);

	if (!bHitThirth) return bOnIsland;

	ComponentBounds = Origin + FVector((BoxExtent.X * 1.05f) * -1.f, (BoxExtent.Y * 1.05f) * -1.f, 0);

	bool bHitFourth = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeBox(FVector(50.f)),
		QueryParams);

	if (!bHitFourth) return bOnIsland;

	bOnIsland = true;
	return bOnIsland;
}


void AUtopPlayer::VillagerTriggered()
{
	VillagerAction = HoverActor;
}

void AUtopPlayer::VillagerStarted()
{
	AActor* OutputActor;

	// Store current position
	TargetHandle = ProjectMouseToGroundIntersection();

	if (VillagerOverlapCheck(OutputActor))
	{
		VillagerSelect(OutputActor);
	}
	else
	{
		if (GetPlayerController())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetPlayerController()->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DragMoveContext, 0);
			}
		}
	}
}

void AUtopPlayer::VillagerCanceled()
{
	RemoveDragMoveContext();

	if (IsValid(Selected))
	{
		if (Selected->Implements<UUtopVillagerInterface>())
		{
			IUtopVillagerInterface::Execute_Action(Selected, VillagerAction);
		}
	}
	VillagerRelease();
	VillagerAction = nullptr;
}

void AUtopPlayer::VillagerCompleted()
{
	RemoveDragMoveContext();

	if (IsValid(Selected))
	{
		if(Selected->Implements<UUtopVillagerInterface>())
		{
			IUtopVillagerInterface::Execute_Action(Selected, VillagerAction);
		}
	}

	VillagerRelease();
	VillagerAction = nullptr;
}

void AUtopPlayer::BuildMoveTriggered()
{
	UpdateBuildAsset();
}

void AUtopPlayer::BuildMoveCompleted()
{
	if (Spawn)
	{
		Spawn->SetActorLocation(UUtopFunctionLibrary::SteppedPosition(Spawn->GetActorLocation()));
	}

}

void AUtopPlayer::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisValue = InputActionValue.Get<FVector2D>();


	AddMovementInput(GetActorForwardVector(), InputAxisValue.Y);

	AddMovementInput(GetActorRightVector(), InputAxisValue.X);
	
}

void AUtopPlayer::Zoom(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();

	ZoomDirection = InputValue;

	UpdateZoom();

	Dof();

}

void AUtopPlayer::Spin(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();

	AddActorLocalRotation(FRotator(0.f, InputValue, 0.f));
}

void AUtopPlayer::DragMove(const FInputActionValue& InputActionValue)
{
	TrackMove();
}




