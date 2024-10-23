// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Utopia/Public/UtopTypes/Resource.h"
#include "Villagers/UtopVillagerInterface.h"
#include "Core/GameMode/UtopResourceInterface.h"
#include "Engine/DataTable.h"
#include "UtopVillager.generated.h"

class UCapsuleComponent;
class UFloatingPawnMovement;
class UBehaviorTree;

USTRUCT(BlueprintType)
struct FJobs : public FTableRowBase
{
	GENERATED_BODY()

public:

	FJobs(){}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "BehaviourTree", MakeStructureDefaultValue = "None"))
	TSoftObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "WorkAnim", MakeStructureDefaultValue = "None"))
	TSoftObjectPtr<UAnimMontage> WorkAnim = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Tool", MakeStructureDefaultValue = "None"))
	TSoftObjectPtr<UStaticMesh> Tool = nullptr;
};


UCLASS()
class UTOPIA_API AUtopVillager : public APawn, public IUtopVillagerInterface, public IUtopResourceInterface
{
	GENERATED_BODY()

public:

	AUtopVillager();

	/* UtopResourceInterface */
	virtual void RemoveResource_Implementation(EResourceType& TargetResource, int32& Value) override;

	virtual void AddResource_Implementation(EResourceType TargetResource, int32 Value) override;

	virtual void GetCurrentResources_Implementation(TMap<EResourceType, int32>& InResources) override;

	virtual void RemoveTargetResources_Implementation(EResourceType TargetResource, int32 Value) override;

	virtual void CheckResource_Implementation(EResourceType TargetResource, bool& bCheck, int32& Num) override;

	/* end UtopResourceInterface */

	virtual void Tick(float DeltaTime) override;

	/* UtopVillagerInterface */

	virtual void ChangeJob_Implementation(FName InNewJob) override;

	virtual void PlayDeliverAnim_Implementation(float& OutDelay) override;

	virtual void PlayWorkAnim_Implementation(float Delay) override;

	virtual void ProgressBuilding_Implementation(float InvestedTimer, float& Out) override;

	virtual void Action_Implementation(AActor* NewParam) override;

	virtual void ReturnToDefaultBT_Implementation() override;

	/* end UtopVillagerInterface */

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Defaults")
	EResourceType ChooseEatResource = EResourceType::ERT_Food;

	UPROPERTY(EditAnywhere, Category = "Defaults")
	int32 ChooseEatResourceQuantity = 3;

	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	// Resource

	EResourceType ResourcesHeld;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Quantity;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> CrateStaticMesh;

	// Job Profile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> TargetRef;

	UPROPERTY(EditAnywhere, Category = "Defaults")
	FName NewJob = FName("Idle");

	UPROPERTY(EditAnywhere, Category = "JobProfile")
	TObjectPtr<UDataTable> JobsData;

	void ResetJobState();

	void StopJob();

	void OnBehaviorTreeLoaded(TSoftObjectPtr<UBehaviorTree> LoadedBehaviorTreeAsset);

	void OnWorkAnimLoaded(TSoftObjectPtr<UAnimMontage> LoadedWorkAnimAsset);

	void OnToolLoaded(TSoftObjectPtr<UStaticMesh> LoadedToolAsset);

	UPROPERTY(EditAnywhere, Category = "JobProfile")
	TObjectPtr<UBehaviorTree> ActiveBehavior;

	UPROPERTY(EditAnywhere, Category = "JobProfile")
	TObjectPtr<UAnimMontage> WorkAnim;

	UPROPERTY(EditAnywhere, Category = "JobProfile")
	TObjectPtr<UStaticMesh> TargetTool;

	void Eat();

	FTimerHandle EatTimerHandle;

	TSoftObjectPtr<USkeletalMesh> HairPick();

	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<USkeletalMesh>> Hairs;

	void SetRandomHairAndColor();

	void OnHairAssetLoaded(TSoftObjectPtr<USkeletalMesh> LoadedHairAsset);

	void ReturnToIdle();

	UPROPERTY(EditAnywhere, Category = "UtopAnimation")
	UAnimMontage* PutDownAnim;

	void PlayVillagerAnim(UAnimMontage* Montage, float Length);

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void OnVillagerMontageDelayFinished();

private:	
	
	// Components 

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Hair;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Tool;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UDecalComponent> Decal;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;


};
