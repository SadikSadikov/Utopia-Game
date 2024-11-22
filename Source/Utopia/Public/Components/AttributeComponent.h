// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamageTakenSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTOPIA_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnDamageTakenSignature OnDamageTakenDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeahtDelegate;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	void RegenHealth(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	bool IsAlive();

protected:

	virtual void BeginPlay() override;

private:

	
	// Attributes

	UPROPERTY(EditAnywhere, Category = "Attribute")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	float HealthRegenRate = 4.f;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	int32 Level = 1;


public:	

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
};
