// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utopia/Public/UtopTypes/Weapon.h"
#include "CombatComponent.generated.h"


class AUtopWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTOPIA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	AActor* GetCombatTarget();

	UFUNCTION(BlueprintCallable)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintCallable)
	void Attack();

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<AUtopWeapon*> EquippedWeapon;

protected:
	
	virtual void BeginPlay() override;

	void PlayAttackMontage();

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<FWeaponParams> WeaponParams;


	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Animation")
	TArray<UAnimMontage*> AttackMontages;


private:

	UPROPERTY(EditAnywhere)
	float DamageAmount = 1.f;

	

public:	

	FORCEINLINE float GetDamageAmount() const { return DamageAmount; }

	FORCEINLINE void SetDamageAmount(float NewDamageAmount) { DamageAmount = NewDamageAmount; }
	

		
};
