// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/UtopItem.h"
#include "Components/SphereComponent.h"
#include "Utopia/PrintStrings.h"


AUtopItem::AUtopItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SetRootComponent(Sphere);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMeshComp");
	ItemMesh->SetupAttachment(GetRootComponent());

}


void AUtopItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AUtopItem::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AUtopItem::OnSphereEndOverlap);
	
}

void AUtopItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AUtopItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


void AUtopItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

