// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/UtopGameModeBase.h"
#include "Core/GameMode/UtopFunctionLibrary.h"
#include "Core/GameMode/UtopGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Spawner/UtopSpawner.h"
#include "Blueprint/UserWidget.h"
#include "UI/Game/UtopLayerGameActivatableWidget.h"
#include "Interactable/UtopBaseInteractable.h"
#include "Core/Save/UtopSaveGame.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Villagers/UtopVillagerInterface.h"
#include "AudioModulationStatics.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "Utopia/PrintStrings.h"
#include "Core/Save/GameInstanceInterface.h"
#include "Kismet/KismetMathLibrary.h"

AUtopGameModeBase::AUtopGameModeBase()
{
}

void AUtopGameModeBase::BeginPlay()
{
	UUtopGameInstance* UtopGameInstance = UUtopFunctionLibrary::GetUtopGameInstance(this);



	if (UtopGameInstance)
	{
		UtopGameInstance->TransitionOut();

		UtopGameInstance->StartGameOffset = UKismetSystemLibrary::GetGameTimeInSeconds(this);

		UKismetRenderingLibrary::ClearRenderTarget2D(this, GrassMoveTexture);

		Resources = UtopGameInstance->GetInitialResources();
	}

	GetSpawnRef();

	if (LayerGameWidget)
	{
		UUtopLayerGameActivatableWidget* Widget = CreateWidget<UUtopLayerGameActivatableWidget>(GetWorld(), LayerGameWidget);

		Widget->AddToViewport();

		HUD = Widget;

		HUD->ActivateWidget();
	}
	
}

void AUtopGameModeBase::BeginBuild_Implementation(TSubclassOf<AUtopBaseInteractable> TargetClass, const TMap<EResourceType, int32>& InResourceCost)
{
}

void AUtopGameModeBase::SwitchToBuildMode_Implementation(bool bSwitchToBuildMode)
{
}

void AUtopGameModeBase::AddUI_Implementation(TSubclassOf<UCommonActivatableWidget> NewParam)
{
	HUD->AddStackItem(NewParam);
}

void AUtopGameModeBase::RemoveCurretUI_Layer_Implementation()
{
	HUD->PullCurrentActiveWidget();
}

void AUtopGameModeBase::ScaleUp_Implementation(float Delay)
{
}

void AUtopGameModeBase::IslandGenComplete_Implementation()
{
	bIslandIsGenerated = true;
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AUtopGameModeBase::IslandGenDelayComleted);

}

void AUtopGameModeBase::IslandGenDelayComleted()
{
	if (bIslandIsGenerated)
	{
		if (IGameInstanceInterface* GameInstanceInterface = Cast<IGameInstanceInterface>(UGameplayStatics::GetGameInstance(this)))
		{
			if (IGameInstanceInterface::Execute_CheckSaveBool(UGameplayStatics::GetGameInstance(this)) && SpawnRef)
			{
				SpawnLoadedInteractables();

				Resources = GameInstanceInterface->GetSave()->Resources;

				SpawnRef->SpawnMeshOnly();

				LoadVillagers();

				UAudioModulationStatics::SetGlobalBusMixValue(this, NewMapBus, 0.f, 0.f);

			}
			else
			{
				BeginAsyncSpawning();

				UAudioModulationStatics::SetGlobalBusMixValue(this, NewMapBus, 1.f, 0.f);
			}
		}
		bIslandIsGenerated = false;
	}
	
}


void AUtopGameModeBase::IslandSeed_Implementation(FRandomStream& Seed)
{
}

void AUtopGameModeBase::SpawningComplete()
{
}

void AUtopGameModeBase::RemoveResource_Implementation(EResourceType& TargetResource, int32& Value)
{
	TargetResource = EResourceType::ERT_None;
	Value = 0;
}

void AUtopGameModeBase::AddResource_Implementation(EResourceType TargetResource, int32 Value)
{
	int32 CacheValue = 0;

	if (Resources.Find(TargetResource))
	{
		CacheValue = Value + *Resources.Find(TargetResource);
	}
	else
	{
		CacheValue = CacheValue + Value;
	}
		

	Resources.Add(TargetResource, CacheValue);
	OnUpdateResourcesDelegate.Broadcast(TargetResource, CacheValue);

	if (UGameplayStatics::GetGameInstance(this)->Implements<UGameInstanceInterface>())
	{
		IGameInstanceInterface::Execute_UpdateAllResource(UGameplayStatics::GetGameInstance(this), Resources);
	}

}

void AUtopGameModeBase::GetCurrentResources_Implementation(TMap<EResourceType, int32>& InResources)
{
	InResources = Resources;
}

void AUtopGameModeBase::RemoveTargetResources_Implementation(EResourceType TargetResource, int32 Value)
{
	if (Resources.Find(TargetResource))
	{
		Resources.Add(TargetResource, *Resources.Find(TargetResource) - Value);

		OnUpdateResourcesDelegate.Broadcast(TargetResource, *Resources.Find(TargetResource));


	}

	if (Resources.Find(EResourceType::ERT_Food)  && *Resources.Find(EResourceType::ERT_Food) <= 0)
	{
		EndGame(false);
	}
}

void AUtopGameModeBase::CheckResource_Implementation(EResourceType TargetResource, bool& bCheck, int32& Num)
{
	bCheck = Resources.Find(TargetResource) ? true : false;
	if (bCheck)
	{
		Num = *Resources.Find(TargetResource);
	}
	

}

void AUtopGameModeBase::GetSpawnRef()
{

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, AUtopSpawner::StaticClass(), OutActors);

	SpawnRef = Cast<AUtopSpawner>(OutActors[0]);
}

void AUtopGameModeBase::SpawnLoadedInteractables()
{
	if (IGameInstanceInterface* GameInstanceInterface = Cast<IGameInstanceInterface>(UGameplayStatics::GetGameInstance(this)))
	{
		for (FSaveInteract SaveInteract : GameInstanceInterface->GetAllInteractables())
		{
			AUtopBaseInteractable* SpawnedActor = GetWorld()->SpawnActor<AUtopBaseInteractable>(SaveInteract.Type, SaveInteract.Location);

			if (SpawnedActor)
			{
				SpawnedActor->bRequireBuild = SaveInteract.Tag == FName("Build");

				SpawnedActor->SetProgressionsState(SaveInteract.Health);

				if (SaveInteract.Type == TownHallRef)
				{
					TownHall = SpawnedActor;
				}
			}

		}

	}
}

void AUtopGameModeBase::BeginAsyncSpawning()
{
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	Manager.RequestAsyncLoad(TownHallRef.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &AUtopGameModeBase::BeginAsyncSpawningCompleted));
}

void AUtopGameModeBase::BeginAsyncSpawningCompleted()
{
	if (SpawnMarkerClass)
	{
		TArray<AActor*> SpawnMarks;
		UGameplayStatics::GetAllActorsOfClass(this, SpawnMarkerClass, SpawnMarks);

		if (SpawnMarks.Num() > 0)
		{
			AActor* SpawnMark = SpawnMarks[FMath::RandRange(0, SpawnMarks.Num() - 1)];

			if (SpawnMark)
			{
				FVector Location = SpawnMark->GetActorLocation();
				FVector RandomLocation = UUtopFunctionLibrary::SteppedPosition(Location);

				TownHall = GetWorld()->SpawnActor<AActor>(TownHallRef.Get(), UKismetMathLibrary::Conv_VectorToTransform(RandomLocation));

				for (int32 i = 0; i < NumberOfVillagers; i++)
				{
					SpawnVillager();
				}

				OnUpdateVillagersDelegate.Broadcast(VillagerCount);

				if (UGameplayStatics::GetGameInstance(this)->Implements<UGameInstanceInterface>())
				{
					IGameInstanceInterface::Execute_UpdateAllVillagers(UGameplayStatics::GetGameInstance(this));
				}

				SpawnRef->SpawnRandom();


			}
		}
	}

}

void AUtopGameModeBase::SpawnVillager() 
{
	FVector Origin;
	FVector BoxExtend;
	
	if (TownHall)
	{
		TownHall->GetActorBounds(false, Origin, BoxExtend);
		FVector CalcTownHallOrigin(Origin + (UKismetMathLibrary::RandomUnitVector() * FMath::Min(BoxExtend.X, BoxExtend.Y) * 2.f));

		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

		if (NavSys)
		{
			FNavLocation RandomLocation;

			NavSys->GetRandomReachablePointInRadius(FVector(CalcTownHallOrigin.X, CalcTownHallOrigin.Y, 0.f), 500.f, RandomLocation);

			UAIBlueprintHelperLibrary::SpawnAIFromClass(GetWorld(), VillagerRef, nullptr, RandomLocation.Location);

			VillagerCount++;
		}
	}

}

void AUtopGameModeBase::EndGame(bool bWin)
{
	if (!bGameEnded)
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FSimpleDelegate::CreateLambda([this, bWin]() {

			HUD->EndGame(bWin);


		}), 3.f, false);

		

		bGameEnded = true;
	}
}

void AUtopGameModeBase::SpawnVillagers(int32 Add)
{
	for (int32 i = 1; i < Add; i++)
	{
		SpawnVillager();
	}

	OnUpdateVillagersDelegate.Broadcast(VillagerCount);

	if (UGameplayStatics::GetGameInstance(this)->Implements<UGameInstanceInterface>())
	{
		IGameInstanceInterface::Execute_UpdateAllVillagers(UGameplayStatics::GetGameInstance(this));
	}
}

void AUtopGameModeBase::LoadVillagers()
{

	if (IGameInstanceInterface* GameInstanceInterface = Cast<IGameInstanceInterface>(UGameplayStatics::GetGameInstance(this)))
	{
		UUtopSaveGame* SaveGameObject = GameInstanceInterface->GetSave();

		for (FVillager Villager : SaveGameObject->Villagers)
		{
			APawn* SpawnedCharacter = UAIBlueprintHelperLibrary::SpawnAIFromClass(this, VillagerRef, nullptr, FVector(Villager.Location.X,
				Villager.Location.Y, 42.f));

			if (SpawnedCharacter->Implements<UUtopVillagerInterface>())
			{
				IUtopVillagerInterface::Execute_ChangeJob(SpawnedCharacter, Villager.Task);
			}
		}

		OnUpdateVillagersDelegate.Broadcast(SaveGameObject->Villagers.Num());

		IGameInstanceInterface::Execute_UpdateAllResource(UGameplayStatics::GetGameInstance(this), Resources);
	}
	
}

