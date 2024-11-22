// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/UtopWeapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utopia/PrintStrings.h"
#include "Characters/Interfaces/DamageableInterface.h"

AUtopWeapon::AUtopWeapon()
{

	WeaponBox = CreateDefaultSubobject<UBoxComponent>("WeaponBox");
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>("BoxTarceStart");
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>("BoxTarceEnd");
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AUtopWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* InOwner, APawn* InInstigator)
{
	SetOwner(InOwner);
	SetInstigator(InInstigator);
	AttachMeshToSocket(InParent, InSocketName);


	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AUtopWeapon::SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type NewCollisionEnabled)
{
	WeaponBox->SetCollisionEnabled(NewCollisionEnabled);
}

void AUtopWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AUtopWeapon::OnWeaponBoxOverlap);

}

void AUtopWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	FAttachmentTransformRules TransfromRules(EAttachmentRule::SnapToTarget, true);

	GetRootComponent()->AttachToComponent(InParent, TransfromRules, InSocketName);
}

void AUtopWeapon::OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	FVector Start = BoxTraceStart->GetComponentLocation();
	FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActor)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	FHitResult HitResult;

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, HalfSize, BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult, true);

	if (HitResult.GetActor())
	{
		printf("Hitten Actor is: %s", *HitResult.GetActor()->GetName());

		if (HitResult.GetActor()->Implements<UDamageableInterface>())
		{
			IDamageableInterface::Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
		}
	}

	IgnoreActor.AddUnique(HitResult.GetActor());
}
