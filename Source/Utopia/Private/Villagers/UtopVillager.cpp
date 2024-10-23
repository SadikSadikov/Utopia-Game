// Fill out your copyright notice in the Description page of Project Settings.


#include "Villagers/UtopVillager.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utopia/PrintStrings.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Core/Save/GameInstanceInterface.h"
#include "GameFramework\GameModeBase.h"


AUtopVillager::AUtopVillager()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	Capsule->SetCapsuleHalfHeight(92.f);
	Capsule->SetCapsuleRadius(30.f);
	Capsule->bDynamicObstacle = true;
	SetRootComponent(Capsule);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMesh->SetupAttachment(GetRootComponent());

	Tool = CreateDefaultSubobject<UStaticMeshComponent>("Tool");
	Tool->SetupAttachment(SkeletalMesh, FName("hand_rSocket"));

	Hair = CreateDefaultSubobject<USkeletalMeshComponent>("Hair");
	Hair->SetupAttachment(SkeletalMesh);

	Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
	Decal->SetupAttachment(GetRootComponent());
	Decal->SetRelativeLocation(FVector(0.f, 0.f, -95.f));
	Decal->SetRelativeRotation(FRotator(-90.f, 360.f, 0.f));
	Decal->SetRelativeScale3D(FVector(0.4375f, 0.34375f, 0.34375f));

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("FloatingPawnMovement");
	FloatingPawnMovement->MaxSpeed = 300.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;


}

void AUtopVillager::RemoveResource_Implementation(EResourceType& TargetResource, int32& Value)
{
	EResourceType CacheResource;
	int32 CacheValue;

	CacheResource = ResourcesHeld;
	CacheValue = Quantity;

	ResourcesHeld = EResourceType::ERT_None;
	Quantity = 0;

	if (Tool)
	{
		Tool->SetVisibility(false);
		Tool->SetStaticMesh(nullptr);
	}
	TargetResource = CacheResource;
	Value = CacheValue;
}

void AUtopVillager::AddResource_Implementation(EResourceType TargetResource, int32 Value)
{
	ResourcesHeld = TargetResource;
	Quantity = Value;

	Tool->SetVisibility(true);
	if (CrateStaticMesh)
	{
		Tool->SetStaticMesh(CrateStaticMesh);
	}

}

void AUtopVillager::GetCurrentResources_Implementation(TMap<EResourceType, int32>& Resources)
{
	EResourceType CacheResource;
	int32 CacheValue;

	CacheResource = ResourcesHeld;
	CacheValue = Quantity;

	ResourcesHeld = EResourceType::ERT_None;
	Quantity = 0;

	Tool->SetVisibility(false);
	Tool->SetStaticMesh(nullptr);

	Resources.Add(CacheResource, CacheValue);
}

void AUtopVillager::RemoveTargetResources_Implementation(EResourceType TargetResource, int32 Value)
{
}

void AUtopVillager::CheckResource_Implementation(EResourceType TargetResource, bool& bCheck, int32& Num)
{
}



void AUtopVillager::BeginPlay()
{
	Super::BeginPlay();

	AddActorWorldOffset(FVector(0.f, 0.f, Capsule->GetUnscaledCapsuleHalfHeight()));

	GetWorld()->GetTimerManager().SetTimer(EatTimerHandle, this, &AUtopVillager::Eat, 24.f, true);

	if (Implements<UUtopVillagerInterface>())
	{
		IUtopVillagerInterface::Execute_ChangeJob(this, FName("Idle"));

	}
	
	

	SetRandomHairAndColor();
	
}

void AUtopVillager::ResetJobState()
{
	StopJob();

	Tool->SetVisibility(false);
	TargetTool = nullptr;

}

void AUtopVillager::StopJob()
{
	Tool->SetVisibility(false);

	SkeletalMesh->GetAnimInstance()->Montage_StopGroupByName(0.f, FName("DefaultGroup"));


	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	Quantity = 0;
}

void AUtopVillager::Eat()
{
	if (UGameplayStatics::GetGameMode(this)->Implements<UUtopResourceInterface>())
	{
		IUtopResourceInterface::Execute_RemoveTargetResources(UGameplayStatics::GetGameMode(this), ChooseEatResource,
			ChooseEatResourceQuantity);
	}
}

TSoftObjectPtr<USkeletalMesh> AUtopVillager::HairPick()
{
	if (Hairs.Num() > 0)
	{
		int32 RandHairNum = FMath::RandRange(0, Hairs.Num() - 1);
		return Hairs[RandHairNum];
	}

	return nullptr;
}

void AUtopVillager::SetRandomHairAndColor()
{

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(HairPick().ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this,
		&AUtopVillager::OnHairAssetLoaded, HairPick()));

}

void AUtopVillager::OnHairAssetLoaded(TSoftObjectPtr<USkeletalMesh> LoadedHairAsset)
{
	if (LoadedHairAsset.IsValid())
	{
		Hair->SetSkeletalMesh(LoadedHairAsset.Get());
	}

	UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Hair);
	if (PrimitiveComp)
	{
		float RandomFloatValue = FMath::RandRange(0.f, 1.f);
		PrimitiveComp->SetCustomPrimitiveDataFloat(0, RandomFloatValue);
	}
}

void AUtopVillager::ReturnToIdle()
{
	IUtopVillagerInterface::Execute_ChangeJob(this, FName("Idle"));
}

void AUtopVillager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUtopVillager::ChangeJob_Implementation(FName InNewJob)
{
	NewJob = InNewJob;
	if (JobsData)
	{
		FJobs* Jobs = JobsData->FindRow<FJobs>(NewJob, FString("JobContext"));

		if (Jobs)
		{
			
			if (Tags.Find(NewJob) == -1)
			{
				TArray<FName> JobTags;
				JobTags.Add(NewJob);

				Tags = JobTags;
				ResetJobState();

				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();


				Streamable.RequestAsyncLoad(Jobs->BehaviorTree.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this,
					&AUtopVillager::OnBehaviorTreeLoaded, Jobs->BehaviorTree));
				

				Streamable.RequestAsyncLoad(Jobs->WorkAnim.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this,
					&AUtopVillager::OnWorkAnimLoaded, Jobs->WorkAnim));
				

				Streamable.RequestAsyncLoad(Jobs->Tool.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this,
					&AUtopVillager::OnToolLoaded, Jobs->Tool));
				


			}

		}
		else
		{
			print("ERROR: Failed to Load Job");
		}

		
		
	}
}

void AUtopVillager::PlayVillagerAnim(UAnimMontage* Montage, float Length)
{
	if (Montage && SkeletalMesh->GetAnimInstance())
	{
		SkeletalMesh->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AUtopVillager::OnMontageEnded);
		SkeletalMesh->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &AUtopVillager::OnMontageBlendingOut);

		SkeletalMesh->GetAnimInstance()->Montage_Play(Montage);

		FTimerHandle Delay;
		GetWorld()->GetTimerManager().SetTimer(Delay, this, &AUtopVillager::OnVillagerMontageDelayFinished, Length, false);

	}
	
}

void AUtopVillager::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		Tool->SetVisibility(false);
		
	}
	
}

void AUtopVillager::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	Tool->SetVisibility(false);

	if (UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &AUtopVillager::OnMontageEnded);
		AnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &AUtopVillager::OnMontageBlendingOut);
	}
}

void AUtopVillager::OnVillagerMontageDelayFinished()
{
	SkeletalMesh->GetAnimInstance()->Montage_StopGroupByName(0.f, FName("DefaultGroup"));
}

void AUtopVillager::OnBehaviorTreeLoaded(TSoftObjectPtr<UBehaviorTree> LoadedBehaviorTreeAsset)
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->RunBehaviorTree(LoadedBehaviorTreeAsset.Get());
		ActiveBehavior = LoadedBehaviorTreeAsset.Get();

		if (TargetRef)
		{
			AIController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), TargetRef);
		}
	}
}

void AUtopVillager::OnWorkAnimLoaded(TSoftObjectPtr<UAnimMontage> LoadedWorkAnimAsset)
{
	WorkAnim = LoadedWorkAnimAsset.Get();
}

void AUtopVillager::OnToolLoaded(TSoftObjectPtr<UStaticMesh> LoadedToolAsset)
{
	TargetTool = LoadedToolAsset.Get();
}


void AUtopVillager::PlayDeliverAnim_Implementation(float& OutDelay)
{
	if (PutDownAnim)
	{
		PlayVillagerAnim(PutDownAnim, 1.f);
			
	}

	OutDelay = 1.f;
}

void AUtopVillager::PlayWorkAnim_Implementation(float Delay)
{
	PlayVillagerAnim(WorkAnim, Delay);

	Tool->SetStaticMesh(TargetTool);
	Tool->SetVisibility(true);
}

void AUtopVillager::ProgressBuilding_Implementation(float InvestedTimer, float& Out)
{
	
}

void AUtopVillager::Action_Implementation(AActor* NewParam)
{
	if (IsValid(NewParam))
	{
		TargetRef = NewParam;
		
		if (NewParam->Tags.Num() > 0 && NewParam->Tags[0].IsValid())
		{
			IUtopVillagerInterface::Execute_ChangeJob(this, NewParam->Tags[0]);

			
			if (UGameplayStatics::GetGameInstance(this)->Implements<UGameInstanceInterface>())
			{
				IGameInstanceInterface::Execute_UpdateAllVillagers(UGameplayStatics::GetGameInstance(this));

			}

		}
	}
}

void AUtopVillager::ReturnToDefaultBT_Implementation()
{
	IUtopVillagerInterface::Execute_ChangeJob(this, FName("Idle"));
}


