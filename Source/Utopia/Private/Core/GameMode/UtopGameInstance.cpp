// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/UtopGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/BaseTransitionUserWidget.h"
#include "Core/Save/UtopSaveGame.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Interactable/UtopBaseInteractable.h"
#include "Kismet/KismetArrayLibrary.h"
#include "AudioModulationStatics.h"
#include "Core/GameMode/UtopFunctionLibrary.h"
#include "Core/GameMode/UtopGameModeBase.h"

UUtopGameInstance::UUtopGameInstance()
{
	SoundMixes.Add(1.f);
	SoundMixes.Add(1.f);
}

// Begin Play
void UUtopGameInstance::Init()
{
	Super::Init();

	if (TransitionWidgetClass)
	{
		TransitionWidget = CreateWidget<UBaseTransitionUserWidget>(this, TransitionWidgetClass);

	}

	LoadGame();
}

void UUtopGameInstance::ScaleUp_Implementation(float Delay)
{
}

void UUtopGameInstance::IslandGenComplete_Implementation()
{
}

void UUtopGameInstance::IslandSeed_Implementation(FRandomStream& Seed)
{
	Seed = SaveRef->Seed;
}

void UUtopGameInstance::SpawningComplete()
{
}

bool UUtopGameInstance::CheckSaveBool_Implementation()
{
	return bHasSave;
}

FRandomStream UUtopGameInstance::GetSeed()
{
	return SaveRef->Seed;
}

TArray<FSaveInteract> UUtopGameInstance::GetAllInteractables()
{
	return SaveRef->Interactables;
}

UUtopSaveGame* UUtopGameInstance::GetSave()
{
	return SaveRef;
}

void UUtopGameInstance::StartNewLevel()
{
}

void UUtopGameInstance::SaveGame()
{
	UGameplayStatics::AsyncSaveGameToSlot(SaveRef, FString("SAVE"), 0,
		FAsyncSaveGameToSlotDelegate::CreateUObject(this, &UUtopGameInstance::OnSaveCompleted));
}

void UUtopGameInstance::UpdateSaveData()
{
}

void UUtopGameInstance::UpdateAllInteractables_Implementation()
{
	if (SaveRef)
	{
		SaveRef->Interactables.Empty();

		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(this, AUtopBaseInteractable::StaticClass(), OutActors);

		int32 Index = 0;
		for (AActor* OutActor : OutActors)
		{
			FSaveInteract SaveInteract;
			SaveInteract.Location = OutActor->GetActorTransform();
			SaveInteract.Type = OutActor->GetClass();
			SaveInteract.Health = Cast<AUtopBaseInteractable>(OutActor)->ProgressionState;
			if (OutActor->Tags.IsValidIndex(0))
			{
				SaveInteract.Tag = OutActor->Tags[0];
			}
			
			SaveRef->Interactables.Add(SaveInteract);
		}

		SaveGame();
	}
}

void UUtopGameInstance::UpdateAllResource_Implementation(const TMap<EResourceType, int32>& InResources)
{
	if (SaveRef)
	{
		SaveRef->Resources = InResources;

		FTimerHandle ResourceTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ResourceTimerHandle, FSimpleDelegate::CreateLambda([this]() {

			SaveGame();

		}), 5.f, false);
	}
}

void UUtopGameInstance::UpdateAllVillagers_Implementation()
{
	if (SaveRef)
	{
		SaveRef->Villagers.Empty();

		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(this, APawn::StaticClass(), OutActors);

		for (AActor* OutActor : OutActors)
		{
			if (OutActor != UGameplayStatics::GetPlayerPawn(this, 0))
			{
				FVillager VillagerData;
				VillagerData.Location = OutActor->GetActorLocation();
				VillagerData.Task = OutActor->Tags.Num() > 0 ? OutActor->Tags[0] : FName("");
				SaveRef->Villagers.Add(VillagerData);
			}
		}

		SaveGame();
	}
}

void UUtopGameInstance::ClearSave_Implementation(bool bClearSeed)
{
	ClearSaveData();

	if (bClearSeed)
	{
		if (SaveRef)
		{
			SaveRef->Seed = UKismetMathLibrary::MakeRandomStream(UKismetMathLibrary::RandomInteger(2147483647));
			
		}
	}
	bHasSave = false;
}

void UUtopGameInstance::LoadLevel()
{
	bHasSave = true;
}

void UUtopGameInstance::PlayMusic(USoundBase* Audio, float Volume, bool Persist)
{
	if (!bMusicPlaying && WinLoseMusic && StopGameMusic)
	{
		UAudioModulationStatics::SetGlobalBusMixValue(this, WinLoseMusic, 0.5f, 0.f);

		UAudioModulationStatics::SetGlobalBusMixValue(this, StopGameMusic, 0.f, 0.f);


		AudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), Audio, Volume, 1.f, 0.f, nullptr, Persist);

		if (AudioComponent)
		{
			AudioComponent->Play();
			bMusicPlaying = true;
		}

	}
}

void UUtopGameInstance::LoadGame()
{
	bHasSave = UGameplayStatics::DoesSaveGameExist(FString("SAVE"), 0);

	if (bHasSave)
	{
		SaveRef = Cast<UUtopSaveGame>(UGameplayStatics::LoadGameFromSlot(FString("SAVE"), 0));
		
	}
	else
	{
		if (SaveGameClass)
		{
			SaveRef = Cast<UUtopSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));

			bMusicPlaying = false;
		}

	}
}

void UUtopGameInstance::StopMusic()
{
	bMusicPlaying = false;
	if (PianoMusic)
	{
		UAudioModulationStatics::SetGlobalBusMixValue(this, PianoMusic, 1.f, 0.f);
	}

}

void UUtopGameInstance::OpenLevel(TSoftObjectPtr<UWorld> InLevel)
{
	Level = InLevel;
	TransitionIn();

	FTimerHandle OpenLevelTimerHandle;

	UWorld* World = GetWorld();

	if (World)
	{
		World->GetTimerManager().SetTimer(OpenLevelTimerHandle, this, &UUtopGameInstance::OnOpenLevelDelay, 1.1f, false);
	}
	
}

void UUtopGameInstance::LoadingUI()
{
	TransitionWidget->AddToViewport();
}

void UUtopGameInstance::ClearSaveData()
{
	if (SaveRef)
	{
		SaveRef->PlayTime = 0.f;
		SaveRef->Villagers.Empty();
		SaveRef->Interactables.Empty();
		SaveRef->Resources.Empty();

		SaveRef->Resources = InitialResources;

		bMusicPlaying = false;
	}
	
}

void UUtopGameInstance::OnSaveCompleted(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	bHasSave = true;
}

void UUtopGameInstance::TransitionIn()
{
	if (TransitionWidget->IsInViewport())
	{
		TransitionWidget->TransIn();
	}
	else
	{
		TransitionWidget->AddToViewport();
		TransitionWidget->TransIn();
	}
}

void UUtopGameInstance::TransitionOut()
{
	if (TransitionWidget->IsInViewport())
	{
		TransitionWidget->TransOut();
	}
	else
	{
		TransitionWidget->AddToViewport();
		TransitionWidget->TransOut();
	}
}

void UUtopGameInstance::OnOpenLevelDelay()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, Level);
}
