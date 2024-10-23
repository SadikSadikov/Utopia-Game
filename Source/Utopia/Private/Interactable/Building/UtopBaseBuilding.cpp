// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Building/UtopBaseBuilding.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Save/GameInstanceInterface.h"



void AUtopBaseBuilding::BeginPlay()
{
	Super::BeginPlay();
}

void AUtopBaseBuilding::ProgressConstruct(float InvestedTime, float& RemaniningConstructionTme)
{
	ProgressionState = (InvestedTime / BuildDifficulty) + ProgressionState;

	if (ProgressionState >= MeshList.Num() - 1)
	{
		ConstructionComplete();
		
		Mesh->SetStaticMesh(MeshList[MeshList.Num() - 1]);
	}
	else
	{
		if (FMath::Floor(ProgressionState) > CurrentStage)
		{
			CurrentStage = FMath::Floor(ProgressionState);

			if (MeshList[CurrentStage])
			{
				Mesh->SetStaticMesh(MeshList[CurrentStage]);
			}
		}
	}

	if (UGameplayStatics::GetGameInstance(this)->Implements<UGameInstanceInterface>())
	{
		IGameInstanceInterface::Execute_UpdateAllInteractables(UGameplayStatics::GetGameInstance(this));

	}

	RemaniningConstructionTme = (MeshList.Num() - 1) - ProgressionState;

}

void AUtopBaseBuilding::SpawnInBuildMode(float Progression)
{
	ProgressionState = Progression;

	Tags.Add(FName("Build"));

	if (MeshList[FMath::TruncToInt32(MeshList.Num() * ProgressionState)])
	{
		Mesh->SetStaticMesh(MeshList[FMath::TruncToInt32(MeshList.Num() * ProgressionState)]);
	}
}

void AUtopBaseBuilding::ConstructionComplete()
{
	Tags.Remove(FName("Build"));

	if (UGameplayStatics::GetGameInstance(this)->Implements<UGameInstanceInterface>())
	{
		IGameInstanceInterface::Execute_UpdateAllInteractables(UGameplayStatics::GetGameInstance(this));

	}

	ReceiveConstructionComplete();
}

void AUtopBaseBuilding::Interact(float& Delay)
{
	Super::Interact(Delay);

}
 