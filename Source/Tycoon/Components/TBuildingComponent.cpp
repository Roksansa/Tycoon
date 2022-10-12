// Fill out your copyright notice in the Description page of Project Settings.


#include "TBuildingComponent.h"

#include "Tycoon/Characters/TPlaceableActor.h"

UTBuildingComponent::UTBuildingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTBuildingComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UTBuildingComponent::UpdatePositionSpawnedObject(const FVector& LocationAtGrid, bool bCanPlaceable)
{
	if (CurrentBuildingPressed == nullptr)
	{
		return;
	}
	CurrentBuildingPressed->SetPlaceable(bCanPlaceable);
	CurrentBuildingPressed->SetActorLocation(LocationAtGrid);
}


ETOnBuildingModeType UTBuildingComponent::GetBuildingModeType() const
{
	return BuildingModeType;
}

void UTBuildingComponent::JustClearSpawnedObject()
{
	if (CurrentBuildingPressed == nullptr)
	{
		return;
	}
	CurrentBuildingPressed->Destroy();
	CurrentBuildingPressed = nullptr;
}

bool UTBuildingComponent::SpawnObject(const FVector& LocationAtGrid, EPlaceableObjectType Type)
{
	if (!TemplateObjects.Find(Type))
	{
		return false;
	}
	JustClearSpawnedObject();
	
	AActor* SpawnedActor = GetWorld()->SpawnActor(TemplateObjects[Type].Get(), &LocationAtGrid);
	if (SpawnedActor != nullptr)
	{
		CurrentBuildingPressed = Cast<ATPlaceableActor>(SpawnedActor);
		return CurrentBuildingPressed != nullptr;
	}
	return false;
}

ATPlaceableActor* UTBuildingComponent::GetSpawnedObject()
{
	return CurrentBuildingPressed;
}

bool UTBuildingComponent::NeedPositionObjectDraw() const
{
	return BuildingMode == ETBuildingMode::On && CurrentBuildingPressed != nullptr;
}

void UTBuildingComponent::ChangeBuildingStateToOff()
{
	JustClearSpawnedObject();
	BuildingMode = ETBuildingMode::Off;
	BuildingModeType = ETOnBuildingModeType::None;
	OnChangeBuildingState.Broadcast(BuildingMode);
}

void UTBuildingComponent::ChangeBuildingState(const FVector& LocationAtGrid, ETBuildingMode Mode, ETOnBuildingModeType ModeType, EPlaceableObjectType Type)
{
	bool bNeedChangeBuildingStateToOff = false;
	DestroyType = Type;
	switch (Mode)
	{
		case ETBuildingMode::On:
		{
			if (Type == EPlaceableObjectType::None || ModeType == ETOnBuildingModeType::None)
			{
				bNeedChangeBuildingStateToOff = true;
				break;
			}
			BuildingMode = Mode;
			if (ModeType != BuildingModeType || CurrentBuildingPressed != nullptr && Type != CurrentBuildingPressed->GetObjectType())
			{
				JustClearSpawnedObject();
				bNeedChangeBuildingStateToOff = ModeType == ETOnBuildingModeType::Create && !SpawnObject(LocationAtGrid, Type);
				BuildingModeType = ModeType;
			}
			break;
		}
		case ETBuildingMode::Off:
		{
			bNeedChangeBuildingStateToOff = true;
			break;
		}
		case ETBuildingMode::RepeatedOn:
		{
			if (BuildingMode != ETBuildingMode::On)
			{
				bNeedChangeBuildingStateToOff = true;
				break;
			}
			EPlaceableObjectType CurrentType = GetPlaceableObjectType();
			CurrentBuildingPressed = nullptr;
			bNeedChangeBuildingStateToOff = BuildingModeType == ETOnBuildingModeType::Create && !SpawnObject(LocationAtGrid, CurrentType);
			break;
		}
		case ETBuildingMode::PlacedToOff:
		{
			CurrentBuildingPressed = nullptr;
			bNeedChangeBuildingStateToOff = true;
			break;
		}
		default: break;
	}
	if (bNeedChangeBuildingStateToOff)
	{
		ChangeBuildingStateToOff();
	}
}

EPlaceableObjectType UTBuildingComponent::GetPlaceableObjectType() const
{
	if (BuildingMode != ETBuildingMode::On || CurrentBuildingPressed == nullptr)
	{
		return BuildingModeType == ETOnBuildingModeType::Destroy ? DestroyType : EPlaceableObjectType::None;
	}
	
	return CurrentBuildingPressed->GetObjectType();
}
