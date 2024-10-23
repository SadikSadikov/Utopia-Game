// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/UtopSpawner.h"
#include "NavigationSystem.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Core/GameMode/UtopFunctionLibrary.h"
#include "Plugin/IslandPluginInterface.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Save/GameInstanceInterface.h"
#include "Plugin/IslandPluginInterface.h"

#include "Utopia/PrintStrings.h"

AUtopSpawner::AUtopSpawner()
{

	PrimaryActorTick.bCanEverTick = true;

}


void AUtopSpawner::BeginPlay()
{
	Super::BeginPlay();

	AsyncLaodClasses();

	if (bAutoSpawn)
	{
		BuildNavSystemAndLoadSoftClasses();
	}
}

void AUtopSpawner::AsyncLaodClasses()
{
	ClassRefIndex = 0;
	bAsyncComplete = false;
	AsyncLoadClassAssets();

}

void AUtopSpawner::AsyncClass()
{
	AsyncLoadClassAssets();
}

void AUtopSpawner::AsyncLoadClassAssets()
{
	if (SpawnTypes.Num() > 0)
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();
		Manager.RequestAsyncLoad(SpawnTypes[ClassRefIndex].ClassRef.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this]() {

			ClassRefIndex++;
			if (ClassRefIndex > SpawnTypes.Num() - 1)
			{
				bAsyncComplete = true;
			}
			else
			{
				AsyncClass();
			}

			}));
	}
	
}

void AUtopSpawner::SpawnMeshOnly()
{
	bActorSwitch = false;

	BuildNavSystemAndLoadSoftClasses();
}

void AUtopSpawner::SpawnRandom()
{
	BuildNavSystemAndLoadSoftClasses();
}

void AUtopSpawner::BuildNavSystemAndLoadSoftClasses()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {

		Counter = 0;
		IndexCounter = 0;

		GetWorld()->GetTimerManager().SetTimer(NavCheckHandle, this, &AUtopSpawner::OnReadyToSpawn, 0.5f, true, -0.5f);

		});



}

void AUtopSpawner::OnReadyToSpawn()
{
	if (!UNavigationSystemV1::GetNavigationSystem(this)->IsNavigationBeingBuilt(this) && bAsyncComplete)
	{
		GetWorld()->GetTimerManager().PauseTimer(NavCheckHandle);

		if (bActorSwitch)
		{
			SpawnAssets(SpawnTypes[IndexCounter].ClassRef.Get(), SpawnTypes[IndexCounter]);

			IndexCounter++;

			if (IndexCounter >= SpawnTypes.Num())
			{
				IndexCounter = 0;
				bActorSwitch = false;
			}
			GetWorld()->GetTimerManager().UnPauseTimer(NavCheckHandle);
		}
		else
		{
			UInstancedStaticMeshComponent* InstancedStaticMesh = NewObject<UInstancedStaticMeshComponent>(this);
			InstancedStaticMesh->SetupAttachment(GetRootComponent());
			InstancedStaticMesh->RegisterComponent();

			InstancedStaticMesh->SetStaticMesh(SpawnInstances[IndexCounter].ClassRef);

			SpawnInst(InstancedStaticMesh, SpawnInstances[IndexCounter].BiomeScale,
				SpawnInstances[IndexCounter].BiomeCount, SpawnInstances[IndexCounter].SpawnPerBiome);

			IndexCounter++;
			if (IndexCounter >= SpawnInstances.Num())
			{
				if (bCallSave)
				{
					FinishedSpawning();
				}
			}
			else
			{
				GetWorld()->GetTimerManager().UnPauseTimer(NavCheckHandle);
			}
		}
	}
}

void AUtopSpawner::SpawnAssets(TSubclassOf<AActor> Class, FSpawnData SpawnParams)
{
	FVector Pos;
	FVector SpawnPos;
	int32 FCounter = 0;
	for (int32 i = 0; i < SpawnParams.BiomeCount; i++)
	{
		UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, FVector::ZeroVector, Pos, 10000.f, NavData);

		for (int32 j = 0; j < UKismetMathLibrary::RandomIntegerFromStream(Seed, SpawnParams.SpawnPerBiome); j++)
		{
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, Pos, SpawnPos, SpawnParams.BiomeScale, NavData);

			FVector SpawnLocation = UUtopFunctionLibrary::SteppedPosition(SpawnPos);
			FRotator SpawnRotator(0.f, FMath::RandRange(0.f, SpawnParams.RandomRotationRange), 0.f);
			FVector SpawnScale = FVector(FMath::RandRange(1.f, SpawnParams.ScaleRange + 1.f));
			FTransform SpawnTransform(SpawnRotator, SpawnLocation, SpawnScale);

			FActorSpawnParameters ActorSpawnParam;
			ActorSpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ActorSpawnParam.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Class, SpawnTransform, ActorSpawnParam);

			FCounter++;

			if (SpawnedActor->Implements<UIslandPluginInterface>())
			{
				//IIslandPluginInterface::Execute_ScaleUp(SpawnedActor, FCounter / TotalCount);
			}

		}
	}

}

void AUtopSpawner::SpawnInst(UInstancedStaticMeshComponent* Class, float InRadius, int32 BiomeCount, int32 MaxSpawn)
{
	FVector Pos;
	FVector SpawnPos;
	int32 FCounter = 0;

	FTransform SpawnTransfrom;
	FVector SpawnLocation;
	FRotator SpawnRotator;
	FVector SpawnScale;
	FVector Distance;

	for (int32 i = 0; i < BiomeCount; i++)
	{
		UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, FVector::ZeroVector, Pos, 10000.f, NavData);

		for(int32 j = 0; j < BiomeCount; j++)
		{
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, Pos, SpawnPos, InRadius, NavData);
			
			SpawnLocation = FVector(SpawnPos.X, SpawnPos.Y, 0.f);
			SpawnRotator = FRotator::ZeroRotator;

			Distance = Pos - SpawnPos;
			SpawnScale = FVector(FMath::Lerp(0.8f, 1.5f, Distance.Length() / InRadius));

			SpawnTransfrom = FTransform(SpawnRotator, SpawnLocation, SpawnScale);

			Class->AddInstance(SpawnTransfrom, true);

			FCounter++;
		}


	}
}

void AUtopSpawner::FinishedSpawning()
{
	if (UGameplayStatics::GetGameInstance(this)->Implements<UGameInstanceInterface>())
	{
		IGameInstanceInterface::Execute_UpdateAllInteractables(UGameplayStatics::GetGameInstance(this));

	}

	if (IIslandPluginInterface* IslandPluginInterface = Cast<IIslandPluginInterface>(UGameplayStatics::GetGameMode(this)))
	{
		IslandPluginInterface->SpawningComplete();
	}
}


void AUtopSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

