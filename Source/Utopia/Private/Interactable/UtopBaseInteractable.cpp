// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/UtopBaseInteractable.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Canvas.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"

AUtopBaseInteractable::AUtopBaseInteractable()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(DefaultSceneRoot);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	Mesh->SetupAttachment(GetRootComponent());

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Box->SetupAttachment(GetRootComponent());


	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>("TimelineComponent");

}

void AUtopBaseInteractable::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {

		if (bEnableGroundBlend)
		{
			if (RenderTargetDraw)
			{
				UCanvas* Canvas;
				FVector2D Size;
				FDrawToRenderTargetContext Context;
				UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RenderTargetDraw, Canvas, Size, Context);

				if (RenderMaterial)
				{
					Canvas->K2_DrawMaterial(RenderMaterial, TransformToTextureScreenPosition(Size), TransformToTextureScreenSize(Size),
						FVector2D::ZeroVector, FVector2D(1.f, 1.f));
				}

				UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);

			}

		}

		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors, AUtopBaseInteractable::StaticClass());

		for (AActor* OverlappingActor : OverlappingActors)
		{
			bool bLocationIsEqual = FVector::Dist(OverlappingActor->GetActorLocation(), GetActorLocation()) <= 5.f;
			bool bActorHasTag = !OverlappingActor->ActorHasTag(FName("PlacementMode"));

			bool bShouldDestroy = bLocationIsEqual && bActorHasTag;

			if (bShouldDestroy)
			{
				Destroy();
			}
		}

		});


}

void AUtopBaseInteractable::PlayWobble(const FVector& InVec)
{

	if (WobbleCurve)
	{
		FOnTimelineFloat ProgressFunction;

		ProgressFunction.BindDynamic(this, &AUtopBaseInteractable::HandleWobbleProgress);

		TimelineComponent->AddInterpFloat(WobbleCurve, ProgressFunction);

	}

	FVector ActorLocation = GetActorLocation();
	FVector NormalizedLocation = (InVec - ActorLocation).GetSafeNormal(0.0001f);

	if (Mesh)
	{
		Mesh->SetVectorParameterValueOnMaterials(FName("WobbleVector"), NormalizedLocation);
	}

	TimelineComponent->Play();



}

void AUtopBaseInteractable::EndWobble()
{
	TimelineComponent->Reverse();
}

void AUtopBaseInteractable::HandleWobbleProgress(float Value)
{
	if (Mesh)
	{
		Mesh->SetScalarParameterValueOnMaterials(FName("Wobble"), Value);
	}
}

void AUtopBaseInteractable::Interact(float& Delay)
{
	ReceiveInteract(Delay);
	Delay = 0.f;
}

void AUtopBaseInteractable::SetProgressionsState(float Progression)
{
	ProgressionState = Progression;

	if (bRequireBuild)
	{
		Tags.Add(FName("Build"));

		if (MeshList[FMath::Floor(ProgressionState)])
		{
			Mesh->SetStaticMesh(MeshList[FMath::Floor(ProgressionState)]);
		}
	}

	ReceiveSetProgressionsState(Progression);
}



FVector2D AUtopBaseInteractable::TransformToTextureScreenPosition(const FVector2D& InVec)
{
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);
	float MinActorBounds = UKismetMathLibrary::Min(BoxExtent.X, BoxExtent.Y);

	float CalcScreenSize = (MinActorBounds / 10000.f) * InVec.X * OutlineDraw;

	FVector CalcScreenPosition = (((GetActorLocation() + 10000.f) / 20000.f) * InVec.X) - (CalcScreenSize / 2.f);

	return FVector2D(CalcScreenPosition.X, CalcScreenPosition.Y);
}

FVector2D AUtopBaseInteractable::TransformToTextureScreenSize(const FVector2D& InVec)
{
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);
	float MinActorBounds = UKismetMathLibrary::Min(BoxExtent.X, BoxExtent.Y);

	float CalcScreenSize = (MinActorBounds / 10000.f) * InVec.X * OutlineDraw;

	return FVector2D(CalcScreenSize, CalcScreenSize);
}

void AUtopBaseInteractable::PlacementMode()
{
	bEnableGroundBlend = false;

	if (MeshList.Num() > 0)
	{
		Mesh->SetStaticMesh(MeshList[0]);

		Tags.Add(FName("PlacementMode"));

		ReceivePlacementMode();
	}


}






