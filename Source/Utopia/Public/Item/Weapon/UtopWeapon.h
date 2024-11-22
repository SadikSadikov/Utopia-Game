// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/UtopItem.h"
#include "UtopWeapon.generated.h"

class UBoxComponent;

UCLASS()
class UTOPIA_API AUtopWeapon : public AUtopItem
{
	GENERATED_BODY()

public:

	AUtopWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* InOwner, APawn* InInstigator);

	UFUNCTION(BlueprintCallable)
	void SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type NewCollisionEnabled);

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> IgnoreActor;

protected:

	virtual void BeginPlay() override;

private:

	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);

	UFUNCTION()
	void OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<USceneComponent> BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FVector HalfSize = FVector(5.f, 5.f, 5.f);

};
