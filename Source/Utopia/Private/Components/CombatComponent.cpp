// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "Characters/UtopBaseCharacter.h"

UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	
}

AActor* UCombatComponent::GetCombatTarget()
{
	return CombatTarget;
}

void UCombatComponent::SetCombatTarget(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


void UCombatComponent::PlayAttackMontage()
{
	if (AttackMontages.Num() > 0)
	{
		int32 SelectRandomMontage = FMath::RandRange(0, AttackMontages.Num() - 1);

		if (AUtopBaseCharacter* Owner = Cast<AUtopBaseCharacter>(GetOwner()))
		{
			if (UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(AttackMontages[SelectRandomMontage]);

			}
		}

	}
}

void UCombatComponent::Attack()
{
	if (CombatTarget)
	{
		PlayAttackMontage();
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

