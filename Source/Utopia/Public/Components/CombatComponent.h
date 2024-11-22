// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utopia/Public/UtopTypes/Weapon.h"
#include "CombatComponent.generated.h"


class AUtopWeapon;
class UGeometryCollection;
class UGeometryCollectionComponent;
struct FChaosBreakEvent;
class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTOPIA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void ReactToHit(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	void OnDeath();

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<AUtopWeapon*> EquippedWeapon;




protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void BreakEvent(const FChaosBreakEvent& BreakEvent);

private:

	// Hit React

	void DirectionalHitReact(const FVector& ImpactPoint);

	void NormalHitReact();

	UPROPERTY(EditAnywhere, Category = "Hit React")
	bool bNormalHitReact = true;

	UPROPERTY(EditAnywhere, Category = "Hit React")
	bool bDirectionalInHitReact = false;

	FVector LastImpactPoint;

	


	// Death

	void NormalDeath();

	void ExplodeDeath(const FVector& ImpactPoint);

	void HandleExplodeDeath();

	void ExplodeMesh();

	UPROPERTY(EditAnywhere, Category = "Death")
	bool bExplodeInDeath = false;

	UPROPERTY(EditAnywhere, Category = "Death", meta = (EditCondition = "bExplodeInDeath"))
	float DelayBeforeSpawnExplodeMesh = 0.5;

	UPROPERTY(EditAnywhere, Category = "Death", meta = (EditCondition = "bExplodeInDeath"))
	UGeometryCollection* ExplodeDeathMesh;

	UPROPERTY()
	UGeometryCollectionComponent* ExplodeDeathMeshComponent;

	bool bDead = false;

	UPROPERTY(EditAnywhere, Category = "Death", meta = (EditCondition = "bExplodeInDeath"))
	float RadialFalloffMagnitude = 10000.f;

	UPROPERTY(EditAnywhere, Category = "Death", meta = (EditCondition = "bExplodeInDeath"))
	float RadialVectorMagnitude = 100000.f;

	UPROPERTY(EditAnywhere, Category = "Death", meta = (EditCondition = "bExplodeInDeath"))
	TObjectPtr<UNiagaraSystem> ExplodeDeathEffect;

	UPROPERTY(EditAnywhere, Category = "Death", meta = (EditCondition = "bExplodeInDeath"))
	float bDelayForExplodeEffect = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Death")
	bool bNormalDeath = true;

	UPROPERTY(EditAnywhere, Category = "Death")
	float DelayBeforeDisappear = 1.5f;

	//

	UPROPERTY(EditAnywhere, Category = "Character Params")
	bool bCanAttack = true;

	UPROPERTY(EditAnywhere, Category = "Character Params", meta = (EditCondition = "bCanAttack"))
	float DamageAmount = 1.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Params")
	TArray<FWeaponParams> Weapons;

	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;

	// Animation

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);

	void PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	void PlayAttackMontage();

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Section")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Section")
	TArray<FName> HitReactMontageSections;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Animation|Section")
	TArray<FName> DeathMontageSections;

	// Motion Warping

	UFUNCTION(BlueprintCallable, Category = "Animation|Motion Warping")
	FVector GetTranslitionCombatTarget();

	UFUNCTION(BlueprintCallable, Category = "Animation|Motion Warping")
	FVector GetRotationCombatTarget();

	UPROPERTY(EditAnywhere, Category = "Animation|Motion Warping")
	float WarpCombatTargetDistance = 75.f;

	// Effect

	void SpawnSystem(UNiagaraSystem* Effect, const FVector& Location);




public:	

	UFUNCTION(BlueprintCallable)
	AActor* GetCombatTarget();

	UFUNCTION(BlueprintCallable)
	void SetCombatTarget(AActor* InCombatTarget);

	FORCEINLINE float GetDamageAmount() const { return DamageAmount; }
	FORCEINLINE void SetDamageAmount(float NewDamageAmount) { DamageAmount = NewDamageAmount; }
	

		
};
