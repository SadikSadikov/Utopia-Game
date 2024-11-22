// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/Interfaces/DamageableInterface.h"
#include "UtopBaseCharacter.generated.h"

class UWidgetComponent;
class UDecalComponent;
class UAttributeComponent;
class UCombatComponent;

UCLASS()
class UTOPIA_API AUtopBaseCharacter : public ACharacter, public IDamageableInterface
{
	GENERATED_BODY()

public:

	AUtopBaseCharacter();

	virtual void Tick(float DeltaTime) override;


	/* Damageable Interace */

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	/* end Damageable Interace */



protected:



	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> CharacterInfoWidget;

	UFUNCTION(BlueprintCallable)
	void ShowInfoWidget();

	UFUNCTION(BlueprintCallable)
	void HideInfoWidget();





private:	

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> Decal;

	UPROPERTY()
	UAttributeComponent* AttributeComponent;

	UPROPERTY()
	UCombatComponent* CombatComponent;


public:

	UFUNCTION(BlueprintCallable)
	UAttributeComponent* GetAttributeComponent();

	UFUNCTION(BlueprintCallable)
	UCombatComponent* GetCombatComponent();


};
