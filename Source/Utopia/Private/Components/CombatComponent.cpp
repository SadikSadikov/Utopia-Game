// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "Characters/UtopBaseCharacter.h"
#include "Item/Weapon/UtopWeapon.h"
#include "Components/SphereComponent.h"
#include "Core/GameMode/UtopFunctionLibrary.h"
#include "Components/AttributeComponent.h"
#include "Field/FieldSystemComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "Utopia/PrintStrings.h"

UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();


	UAttributeComponent* AttributeComponent = UUtopFunctionLibrary::GetAttributeComponentByClass(GetOwner());
	if (AttributeComponent)
	{
		AttributeComponent->OnDeahtDelegate.AddDynamic(this, &UCombatComponent::OnDeath);
	}



	if (Weapons.Num() > 0)
	{
		if (AUtopBaseCharacter* Owner = Cast<AUtopBaseCharacter>(GetOwner()))
		{
			for (const FWeaponParams& Weapon : Weapons)
			{
				AUtopWeapon* WeaponObject = GetWorld()->SpawnActor<AUtopWeapon>(Weapon.WeaponClass);
				WeaponObject->Equip(Owner->GetMesh(), Weapon.SocketName, Owner, Owner);
				EquippedWeapon.Add(WeaponObject);
			}
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

AActor* UCombatComponent::GetCombatTarget()
{
	return CombatTarget;
}

void UCombatComponent::SetCombatTarget(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

void UCombatComponent::PlayAttackMontage()
{
	PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

FVector UCombatComponent::GetTranslitionCombatTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetOwner()->GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();

	TargetToMe *= WarpCombatTargetDistance;

	return CombatTargetLocation + TargetToMe;
}

FVector UCombatComponent::GetRotationCombatTarget()
{
	if (CombatTarget == nullptr) return FVector();

	return CombatTarget->GetActorLocation();
}

void UCombatComponent::SpawnSystem(UNiagaraSystem* Effect, const FVector& Location)
{
	if (Effect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Effect, Location);
	}

}

void UCombatComponent::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	if (ACharacter* OwnerObject = Cast<ACharacter>(GetOwner()))
	{
		if (UAnimInstance* AnimInstance = OwnerObject->GetMesh()->GetAnimInstance())
		{
			if (Montage)
			{
				AnimInstance->Montage_Play(Montage);
				AnimInstance->Montage_JumpToSection(SectionName, Montage);
			}
			
		}
	}
	
}

void UCombatComponent::ReactToHit(const FVector& ImpactPoint)
{
	LastImpactPoint = ImpactPoint;

	if (bNormalHitReact)
	{
		NormalHitReact();
	}

	if (bDirectionalInHitReact)
	{
		DirectionalHitReact(ImpactPoint);
	}

}

void UCombatComponent::DirectionalHitReact(const FVector& ImpactPoint)
{
	bNormalHitReact = false;

	const FVector Forward = GetOwner()->GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Location
	const FVector LoweredImpactPoint(ImpactPoint.X, ImpactPoint.Y, GetOwner()->GetActorLocation().Z);
	const FVector ToHit = (LoweredImpactPoint - GetOwner()->GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward| |THit| * cos(Theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(Theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	// Take the inverse cosine ( arc - cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);

	// Convert From Radians To Degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// If Cross Product point down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	printf("Theta - %f", Theta);

	FName SectionName("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		SectionName = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		SectionName = FName("FromRight");
	}

	PlayMontageSection(HitReactMontage, SectionName);
	
}

void UCombatComponent::NormalHitReact()
{
	bDirectionalInHitReact = false;
	PlayRandomMontageSection(HitReactMontage, HitReactMontageSections);
}

void UCombatComponent::OnDeath()
{
	if (bNormalDeath)
	{
		NormalDeath();
	}

	if (bExplodeInDeath)
	{
		ExplodeDeath(LastImpactPoint);
	}


}

void UCombatComponent::NormalDeath()
{
	bExplodeInDeath = false;
	PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

void UCombatComponent::ExplodeDeath(const FVector& ImpactPoint)
{

	bNormalDeath = false;
	// For now use bDead for hitting once when died
	if (!bDead)
	{
		bDead = true;
		ReactToHit(ImpactPoint);

		FTimerHandle DeathTimer;
		GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &UCombatComponent::HandleExplodeDeath, DelayBeforeSpawnExplodeMesh);

	}

	

}

void UCombatComponent::HandleExplodeDeath()
{
	if (ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner()))
	{
		OwningCharacter->GetMesh()->SetVisibility(false);
	}

	//


	ExplodeDeathMeshComponent = NewObject<UGeometryCollectionComponent>(GetOwner());

	if (ExplodeDeathMesh)
	{
		ExplodeDeathMeshComponent->SetRestCollection(ExplodeDeathMesh);
	}
	ExplodeDeathMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	ExplodeDeathMeshComponent->SetupAttachment(GetOwner()->GetRootComponent());
	ExplodeDeathMeshComponent->RegisterComponent();

	ExplodeDeathMeshComponent->SetEnableGravity(true);
	ExplodeDeathMeshComponent->SetSimulatePhysics(true);

	if (ExplodeDeathMeshComponent && ExplodeDeathMeshComponent->IsRegistered())
	{
		ExplodeDeathMeshComponent->OnChaosBreakEvent.AddDynamic(this, &UCombatComponent::BreakEvent);
	}
	

	//

	if (ExplodeDeathMeshComponent->OnChaosBreakEvent.IsBound())
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCombatComponent::ExplodeMesh, 2.f);
	}
	
	


}

void UCombatComponent::ExplodeMesh()
{
	UFieldSystemComponent* FieldSystemComponent = NewObject<UFieldSystemComponent>(GetOwner());
	FieldSystemComponent->SetupAttachment(GetOwner()->GetRootComponent());
	FieldSystemComponent->RegisterComponent();

	URadialFalloff* RadialFalloff = NewObject<URadialFalloff>();
	RadialFalloff->Magnitude = RadialFalloffMagnitude;
	RadialFalloff->MinRange = 0.8;
	RadialFalloff->MaxRange = 1.f;
	RadialFalloff->Default = 0.f;
	RadialFalloff->Radius = 200.f;
	RadialFalloff->Position = GetOwner()->GetActorLocation();
	RadialFalloff->Falloff = EFieldFalloffType::Field_FallOff_None;

	URadialVector* RadialVector = NewObject<URadialVector>();
	RadialVector->Magnitude = RadialVectorMagnitude;
	RadialVector->Position = GetOwner()->GetActorLocation();


	FieldSystemComponent->ApplyPhysicsField(true, EFieldPhysicsType::Field_ExternalClusterStrain, nullptr, RadialFalloff);
	FieldSystemComponent->ApplyPhysicsField(true, EFieldPhysicsType::Field_LinearForce, nullptr, RadialVector);

	//
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]() {

		SpawnSystem(ExplodeDeathEffect, GetOwner()->GetActorLocation());

		}), bDelayForExplodeEffect, false);
	

	//

	GetOwner()->SetLifeSpan(DelayBeforeDisappear);
}

void UCombatComponent::BreakEvent(const FChaosBreakEvent& BreakEvent)
{
	//TODO:: Break Callback not working
}

void UCombatComponent::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return;

	const int32 Selection = FMath::RandRange(0, SectionNames.Num() - 1);

	PlayMontageSection(Montage, SectionNames[Selection]);
}

void UCombatComponent::Attack()
{
	PlayAttackMontage();
}



