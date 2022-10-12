// Fill out your copyright notice in the Description page of Project Settings.


#include "TResourceManager.h"


DEFINE_LOG_CATEGORY_STATIC(LogFResource, Verbose, Verbose);

FResource::FResource() : FResource(0){}

FResource::FResource(int32 CountInit)
{
	Count = CountInit;
}

int32 FTResourceDesc::GetCount() const
{
	return Count;
}

ETResourceType FTResourceDesc::GetResourceType() const
{
	return Type;
}

float FTResourceIncomeDesc::GetSpawnRewardTime() const
{
	return SpawnRewardTime;
}

int32 FTResourceIncomeDesc::GetMaxDeposit() const
{
	return MaxDeposit;
}

//FResource
bool FResource::Add(int32 Value)
{
	const int32 CurrentCount = Count;
	Count = FMath::Clamp(MIN_int32,Count+Value,MAX_int32);
	return Count != CurrentCount;
}

int32 FResource::GetCount() const
{
	return Count;
}

ETResourceType FResource::GetType() const
{
	return Type;
}

//end FResource


//ATResourceManager
ATResourceManager::ATResourceManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATResourceManager::BeginPlay()
{
	Super::BeginPlay();
	CurrentResources.Add(ETResourceType::Wood, FResource(150));
}

int32 ATResourceManager::GetCount(ETResourceType Type) const
{
	if (CurrentResources.Contains(Type))
	{
		return CurrentResources[Type].GetCount();
	}
	return 0;
}

void ATResourceManager::Add(ETResourceType Type, int32 Value)
{
	if (!CurrentResources.Contains(Type))
	{
		return;
	}

	if (CurrentResources[Type].Add(Value))
	{
		OnUpdateResourceCount.Broadcast(Type, CurrentResources[Type].GetCount());
	}
}

void ATResourceManager::Spend(const TArray<FTResourceDesc>& Price)
{
	for (const FTResourceDesc& TResourceDesc : Price)
	{
		Add(TResourceDesc.GetResourceType(), -TResourceDesc.GetCount());
	}
}

void ATResourceManager::CheckResources()
{
	if (!OnUpdateResourceCount.IsBound())
	{
		return;
	}
	
	for (auto CurrentResource : CurrentResources)
	{
		OnUpdateResourceCount.Broadcast(CurrentResource.Key, CurrentResource.Value.GetCount());
	}
}

bool ATResourceManager::CanBuy(const TArray<FTResourceDesc>& Price)
{
	bool Result = true;
	for (const FTResourceDesc& TResourceDesc : Price)
	{
		if (!CurrentResources.Contains(TResourceDesc.GetResourceType()))
		{
			return false;
		}
		Result = Result && CurrentResources[TResourceDesc.GetResourceType()].GetCount() - TResourceDesc.GetCount() >= 0;
	}
	
	return Result;
}
