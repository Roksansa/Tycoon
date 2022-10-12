// Fill out your copyright notice in the Description page of Project Settings.


#include "TPlaceableActor.h"

EPlaceableObjectType ATPlaceableActor::GetObjectType() const
{
	return PlaceableObjectType;
}


void ATPlaceableActor::SetPlaceable(bool bCanPlaceAtLocation)
{
	if (bCanPlaceAtLocation != bIsCurrentPlaceable)
	{
		bIsCurrentPlaceable = bCanPlaceAtLocation;
		if (OnSwitchPlaceable.IsBound())
		{
			FString GetLocationOnGridLog = FString::Format(TEXT("IN !!!!!!!!!!!!!!!!!!!!!!!! {0} - {1} "), {bCanPlaceAtLocation, bIsCurrentPlaceable});
			UE_LOG(LogEngine, Error, TEXT("%s 0"), *GetLocationOnGridLog);
			OnSwitchPlaceable.Broadcast(bIsCurrentPlaceable);
		}
	}
}

// Sets default values
ATPlaceableActor::ATPlaceableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshes.Reserve(4);
}

TArray<UStaticMeshComponent*> ATPlaceableActor::GetStaticMeshes() const
{
	return StaticMeshes;
}

const TArray<FTResourceDesc>& ATPlaceableActor::GetPrice() const
{
	return Price;
}

const FTResourceIncomeDesc& ATPlaceableActor::GetIncome() const
{
	return Income;
}

int32 ATPlaceableActor::GetIncomeCount() const
{
	return CurrentDepositIncome.Get<CURRENT_RESOURCES>();
}

void ATPlaceableActor::StartBuilding()
{
	SetPlaceable(true);
	SetHaveResources(false);
	BuildState = ETBuildState::StartedBuild;
	//mb add building in loan
	/*for (const FTResourceDesc& PriceInfo : Price)
	{
		const bool bFound = CurrentDepositBuild.Contains(PriceInfo.GetResourceType());
		int32 CurrentResources = 0;
		if (bFound)
		{
			CurrentResources = CurrentDepositBuild[PriceInfo.GetResourceType()].Get<CURRENT_RESOURCES>() + PriceInfo.GetCount();
		}
		CurrentDepositBuild.Add(PriceInfo.GetResourceType(), MakeTuple(CurrentResources, PriceInfo.GetCount()));
	}*/
	if (CurrentDepositBuild.Num() < 1)
	{
		FinishBuilding();
	}
}

bool ATPlaceableActor::GetCanCollectIncome() const
{
	return bCanGenerateIncome && bHaveResources;
}

bool ATPlaceableActor::CollectedIncome(int32& CountReward)
{
	if (GetCanCollectIncome())
	{
		CountReward = GetIncomeCount();
		CurrentDepositIncome = MakeTuple(0, Income.GetMaxDeposit());
		SetHaveResources(false);
		StartGenerateIncome();
		return true;
	}
	return false;
}

// Called when the game starts or when spawned
void ATPlaceableActor::BeginPlay()
{
	Super::BeginPlay();
	GetComponents<UStaticMeshComponent>(StaticMeshes);
	bIsCurrentPlaceable = true;
	if (OnSwitchPlaceable.IsBound())
	{
		OnSwitchPlaceable.Broadcast(bIsCurrentPlaceable);
	}
}


void ATPlaceableActor::FinishBuilding()
{
	//anim and ect effect
	//mb need new timer for anim wait
	BuildState = ETBuildState::DoneBuild;
	if (bCanGenerateIncome)
	{
		CurrentDepositIncome = MakeTuple(0, Income.GetMaxDeposit());
	}
	StartGenerateIncome();
}

void ATPlaceableActor::StartGenerateIncome()
{
	if (!bCanGenerateIncome)
	{
		return;
	}
	
	if (Income.GetSpawnRewardTime() > 0)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(IncomeTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(IncomeTimerHandle, this, &ATPlaceableActor::FinishGenerateIncome, Income.GetSpawnRewardTime(), false);
		}
	}
	else
	{
		FinishGenerateIncome();
	} 
}

void ATPlaceableActor::FinishGenerateIncome()
{
	//anim and ect effect
	//mb need new timer for anim wait
	GetWorld()->GetTimerManager().ClearTimer(IncomeTimerHandle);
	const int CurrentResources = FMath::Clamp(CurrentDepositIncome.Get<CURRENT_RESOURCES>() + Income.GetCount(), 0, CurrentDepositIncome.Get<MAX_RESOURCES>());
	CurrentDepositIncome = MakeTuple(CurrentResources, CurrentDepositIncome.Get<MAX_RESOURCES>());
	SetHaveResources(CurrentDepositIncome.Get<CURRENT_RESOURCES>() > 0);
	if (CurrentResources <= CurrentDepositIncome.Get<MAX_RESOURCES>())
	{
		StartGenerateIncome();
	}
}

void ATPlaceableActor::SetHaveResources(bool bNewHaveResources)
{
	if (bHaveResources == bNewHaveResources)
	{
		return;
	}

	bHaveResources = bNewHaveResources;
	if (OnChangeDepositAvailable.IsBound())
	{
		OnChangeDepositAvailable.Broadcast(this, bHaveResources);
	}
}
