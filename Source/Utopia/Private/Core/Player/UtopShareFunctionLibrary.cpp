// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Player/UtopShareFunctionLibrary.h"

FVector UUtopShareFunctionLibrary::ConvertToSteppedPos(const FVector& A)
{
	FVector DevideVector = A / FVector(200.f);

	return FVector(FMath::RoundToInt32(DevideVector.X) * 200.f, FMath::RoundToInt32(DevideVector.Y) * 200.f, 0.f);
}
