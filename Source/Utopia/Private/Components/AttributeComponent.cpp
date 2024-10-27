// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"


UAttributeComponent::UAttributeComponent()
{

	PrimaryComponentTick.bCanEverTick = true;


}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UAttributeComponent::TakeDamage(float DamageAmount)
{
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);	

	OnDamageTakenDelegate.Broadcast();

	if (!IsAlive())
	{

		OnDeahtDelegate.Broadcast();
	}
}

void UAttributeComponent::RegenHealth(float DeltaTime)
{
	Health = FMath::Clamp(Health + HealthRegenRate * DeltaTime, 0.f, MaxHealth);
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}
