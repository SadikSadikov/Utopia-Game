// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Resources/UtopBaseResource.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Save/GameInstanceInterface.h"

AUtopBaseResource::AUtopBaseResource()
{
}

void AUtopBaseResource::RemoveResource_Implementation(EResourceType& TargetResource, int32& Value)
{
	EndWobble();

	if (ResourceAmount == -1.f)
	{
		TargetResource = ResourceType;
		Value = CollectionValue;

	}
	else
	{
		ResourceAmount = FMath::Max<float>(ResourceAmount - CollectionValue, 0.f);
		if (ResourceAmount <= 0.f)
		{
			Death();
			TargetResource = ResourceType;
			Value = CollectionValue;
		}
		else
		{
			TargetResource = ResourceType;
			Value = CollectionValue;
		}
	}
}

void AUtopBaseResource::AddResource_Implementation(EResourceType TargetResource, int32 Value)
{
}

void AUtopBaseResource::GetCurrentResources_Implementation(TMap<EResourceType, int32>& InResources)
{
}

void AUtopBaseResource::RemoveTargetResources_Implementation(EResourceType TargetResource, int32 Value)
{
}

void AUtopBaseResource::CheckResource_Implementation(EResourceType TargetResource, bool& bCheck, int32& Num)
{
}

void AUtopBaseResource::ScaleUp_Implementation(float Delay)
{
	Mesh->SetHiddenInGame(true);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AUtopBaseResource::OnScaleUpDelayCompleted, Delay);
}



void AUtopBaseResource::OnScaleUpDelayCompleted()
{
	if (TimelineCurve)
	{
		FOnTimelineFloat ProgressFunction;

		ProgressFunction.BindDynamic(this, &AUtopBaseResource::OnTimelineUpdate);
		TimelineComponent->AddInterpFloat(TimelineCurve, ProgressFunction);

	}

	Mesh->SetRelativeScale3D(FVector::ZeroVector);
	Mesh->SetHiddenInGame(false);
	TimelineComponent->PlayFromStart();
}

void AUtopBaseResource::IslandGenComplete_Implementation()
{
}

void AUtopBaseResource::IslandSeed_Implementation(FRandomStream& Seed)
{
}

void AUtopBaseResource::SpawningComplete()
{
}


void AUtopBaseResource::BeginPlay()
{
	Super::BeginPlay();

}

void AUtopBaseResource::OnTimelineUpdate(float Value)
{
	Mesh->SetRelativeScale3D(FVector(Value));
}

void AUtopBaseResource::Death()
{
	Destroy();

	if (UGameplayStatics::GetGameInstance(this)->Implements<UGameInstanceInterface>())
	{
		IGameInstanceInterface::Execute_UpdateAllInteractables(UGameplayStatics::GetGameInstance(this));

	}
}

void AUtopBaseResource::Interact(float& Delay)
{
	Super::Interact(Delay);

}

void AUtopBaseResource::SetProgressionsState(float Progression)
{
	Super::SetProgressionsState(Progression);
}


