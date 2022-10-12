// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Controllers/TResourceManager.h"
#include "GameFramework/Actor.h"

#include "TPlaceableActor.generated.h"


UENUM(BlueprintType)
enum class EPlaceableObjectType : uint8
{
	None = 0,
	Road,
	Building,
	MAX UMETA(Hidden)
};

UENUM()
enum class ETBuildState : uint8
{
	None = 0,
	NotStartedBuild,
	StartedBuild,
	DoneBuild,
	MAX UMETA(Hidden)
};

class ATPlaceableActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchPlaceable, bool, bIsPlaceable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeDepositAvailable, ATPlaceableActor*, Actor, bool, bDepositAvailable);

UCLASS(Abstract, NotBlueprintable)
class TYCOON_API ATPlaceableActor : public AActor
{
	GENERATED_BODY()
public:
	ATPlaceableActor();
	EPlaceableObjectType GetObjectType() const;
	void SetPlaceable(bool bCanPlaceAtLocation);

	TArray<UStaticMeshComponent*> GetStaticMeshes() const;
	const TArray<FTResourceDesc>& GetPrice() const;
	const FTResourceIncomeDesc& GetIncome() const;
	int32 GetIncomeCount() const;
	void StartBuilding();

	bool GetCanCollectIncome() const;

	bool CollectedIncome(int32& CountReward);

protected:
	
	virtual void BeginPlay() override;
	virtual void StartGenerateIncome();
	virtual void FinishGenerateIncome();
	virtual void FinishBuilding();

	UPROPERTY(BlueprintAssignable)
	FOnSwitchPlaceable OnSwitchPlaceable;

	UPROPERTY(BlueprintAssignable)
	FOnChangeDepositAvailable OnChangeDepositAvailable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameInfo)
	EPlaceableObjectType PlaceableObjectType = EPlaceableObjectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Meshes)
	TArray<UStaticMeshComponent*> StaticMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameInfo)
	TArray<FTResourceDesc> Price;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameInfo)
	bool bCanGenerateIncome = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameInfo, meta = (EditCondition = "bCanGenerateIncome == true"))
	FTResourceIncomeDesc Income;
	
	bool bIsCurrentPlaceable = true;

	FTimerHandle BuildTimerHandle;
	FTimerHandle IncomeTimerHandle;

	ETBuildState BuildState = ETBuildState::NotStartedBuild;
	TMap<ETResourceType, TTuple<int32, int32>> CurrentDepositBuild;

	TTuple<int32, int32> CurrentDepositIncome;

	//literals for typle CurrentDepositIncome
	constexpr static int32 CURRENT_RESOURCES = 0;
	constexpr static int32 MAX_RESOURCES = 1;

	bool bHaveResources = false;
	void SetHaveResources(bool bNewHaveResources);
};
