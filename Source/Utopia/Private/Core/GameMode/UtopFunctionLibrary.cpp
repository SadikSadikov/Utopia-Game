// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/UtopFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

FVector UUtopFunctionLibrary::SteppedPosition(const FVector& NewParam)
{
	return FVector(FMath::RoundToInt32(NewParam.X / 200.0) * 200.f, FMath::RoundToInt32(NewParam.Y / 200.f) * 200.f, 0.f);
}

AUtopGameModeBase* UUtopFunctionLibrary::GetUtopGameMode(UObject* WorldContextObject)
{
	return Cast<AUtopGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
}

UUtopGameInstance* UUtopFunctionLibrary::GetUtopGameInstance(UObject* WorldContextObject)
{
	return Cast<UUtopGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}
