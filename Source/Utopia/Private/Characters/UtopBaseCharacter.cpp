// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/UtopBaseCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/DecalComponent.h"
#include "UI/UtopUserWidget.h"
#include "Components/AttributeComponent.h"
#include "Components/CombatComponent.h"

#include "DrawDebugHelpers.h"
#include "Utopia/PrintStrings.h"


AUtopBaseCharacter::AUtopBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
	Decal->SetupAttachment(GetRootComponent());
	Decal->SetRelativeLocation(FVector(0.f, 0.f, -95.f));
	Decal->SetRelativeRotation(FRotator(-90.f, 360.f, 0.f));
	Decal->SetRelativeScale3D(FVector(0.4375f, 0.34375f, 0.34375f));

	CharacterInfoWidget = CreateDefaultSubobject<UWidgetComponent>("WidgetComp");
	CharacterInfoWidget->SetupAttachment(GetRootComponent());

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

}



void AUtopBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	HideInfoWidget();

	if (UUtopUserWidget* UtopUserWidget = Cast<UUtopUserWidget>(CharacterInfoWidget->GetUserWidgetObject()))
	{
		UtopUserWidget->SetWidgetOwner(this);
	}

	
}

void AUtopBaseCharacter::ShowInfoWidget()
{
	if (CharacterInfoWidget)
	{
		CharacterInfoWidget->SetVisibility(true);
	}
}

void AUtopBaseCharacter::HideInfoWidget()
{
	if (CharacterInfoWidget)
	{
		CharacterInfoWidget->SetVisibility(false);
	}
}

UAttributeComponent* AUtopBaseCharacter::GetAttributeComponent()
{
	if (AttributeComponent == nullptr)
	{
		AttributeComponent = GetComponentByClass<UAttributeComponent>();
	}

	return AttributeComponent;
}

UCombatComponent* AUtopBaseCharacter::GetCombatComponent()
{
	if (CombatComponent == nullptr)
	{
		CombatComponent = GetComponentByClass<UCombatComponent>();
	}

	return CombatComponent;
}

void AUtopBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUtopBaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	ShowInfoWidget();

	if (GetAttributeComponent() && GetCombatComponent())
	{
		GetAttributeComponent()->TakeDamage(Hitter->GetComponentByClass<UCombatComponent>()->GetDamageAmount());

		if (GetAttributeComponent()->IsAlive())
		{
			GetCombatComponent()->ReactToHit(ImpactPoint);
		}
		else
		{
			GetCombatComponent()->OnDeath();

		}
	}
}


