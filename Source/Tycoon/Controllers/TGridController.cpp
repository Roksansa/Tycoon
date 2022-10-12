// Fill out your copyright notice in the Description page of Project Settings.


#include "TGridController.h"

#include "Tycoon/Characters/TCellObject.h"
#include "Tycoon/Characters/TPlaceableActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogGridController, Verbose, Verbose);

// Sets default values
ATGridController::ATGridController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATGridController::BeginPlay()
{
	Super::BeginPlay();
	Cells.Empty();
	Cells.Reserve(HorizontalSize * VerticalSize);
	CellObjects.Empty();
	CellObjects.Reserve(HorizontalSize * VerticalSize);
	
	checkf(TemplateSpawnActor != nullptr, TEXT("ATGridController::BeginPlay() Need TemplateSpawnActor"));

	//x -y in game position
	const FVector TopLeftCornerCenterLocation = FVector(CellSize * VerticalSize * 0.5f - CellSize * 0.5f, -CellSize * HorizontalSize * 0.5f + CellSize * 0.5f,0.f);
	for (int i = 0; i < VerticalSize; i++)
	{
		for (int j = 0; j < HorizontalSize; j++)
		{
			const FVector CurrentLocation = TopLeftCornerCenterLocation - FVector(CellSize * i, -CellSize * j, 0.f);
			AActor* SpawnedActor = GetWorld()->SpawnActor(TemplateSpawnActor.Get(), &CurrentLocation);
			if (SpawnedActor != nullptr)
			{
				Cells.Add(SpawnedActor);
				CellObjects.Add(NewObject<UTCellObject>());
			}
		}
	}
	TopLeftCornerLocation = FVector(CellSize * VerticalSize * 0.5f, -CellSize * HorizontalSize * 0.5f,0.f);
	OnEndedBeginPlay.ExecuteIfBound();
	InitAfterBeginPlay();
}

const FVector ATGridController::GetLocationOnGrid(const FVector& Location) const
{
	int HorizontalPosition = 0;
	int VerticalPosition = 0;
	const bool bIsIncludedInterval = GetPointOnGrid(Location, HorizontalPosition, VerticalPosition);
	
	bool IsBusyCell = false;
	if (bIsIncludedInterval)
	{
		IsBusyCell = CellObjects[VerticalPosition * HorizontalSize + HorizontalPosition]->GetIsBusy();
	}
	FString GetLocationOnGridLog = FString::Format(TEXT("Grid {0} - {1} -- {2} --- {3} -- {4} "), {VerticalPosition, HorizontalPosition, bIsIncludedInterval, Location.ToString()});
	UE_LOG(LogGridController, Verbose, TEXT("%s"), *GetLocationOnGridLog);
	return bIsIncludedInterval && !IsBusyCell ? Cells[VerticalPosition * HorizontalSize + HorizontalPosition]->GetActorLocation() : FVector(Location.X, Location.Y, 0.f);
}

bool ATGridController::GetPointOnGrid(const FVector& Location, int& HorizontalPosition, int& VerticalPosition) const
{
	HorizontalPosition = FMath::Clamp((int)((Location.Y - TopLeftCornerLocation.Y)/CellSize),0, HorizontalSize - 1);
	VerticalPosition = FMath::Clamp((int)((TopLeftCornerLocation.X - Location.X) / CellSize), 0, VerticalSize - 1);
	return FMath::FloorToInt((Location.Y - TopLeftCornerLocation.Y) / CellSize) == HorizontalPosition
									&& FMath::FloorToInt((TopLeftCornerLocation.X - Location.X) / CellSize) == VerticalPosition;
}

bool ATGridController::CanPlaceAtLocation(EPlaceableObjectType Type, const FVector& Location) const
{
	int HorizontalPosition = 0;
	int VerticalPosition = 0;
	const bool bIsIncludedInterval = GetPointOnGrid(Location, HorizontalPosition, VerticalPosition);
	
	bool IsBusyCell = false;
	if (bIsIncludedInterval)
	{
		IsBusyCell = CellObjects[VerticalPosition * HorizontalSize + HorizontalPosition]->GetIsBusy();
	}
	bool IsCorrectNeighbours = false;
	if (bIsIncludedInterval && !IsBusyCell)
	{
		IsCorrectNeighbours = CheckCorrectNeighbours(Type, HorizontalPosition, VerticalPosition);
	}
	FString GetLocationOnGridLog = FString::Format(TEXT("Result Place bIsIncludedInterval && !IsBusyCell && IsCorrectNeighbours  {0} -  IsCorrectNeighbours {1}"), {bIsIncludedInterval && !IsBusyCell && IsCorrectNeighbours, IsCorrectNeighbours});
	UE_LOG(LogGridController, Verbose, TEXT("%s Can place"), *GetLocationOnGridLog);
	return bIsIncludedInterval && !IsBusyCell && IsCorrectNeighbours;
}

const FVector ATGridController::GetCenterOnGrid() const
{
	FVector Center = FVector::ZeroVector;
	if (Cells.Num() < 1)
	{
		return Center;
	}
	int AddDiff = VerticalSize%2 == 0 ? HorizontalSize * 0.5f : 0;
	Center = Cells[VerticalSize * HorizontalSize * 0.5f + AddDiff]->GetActorLocation();
	return Center;
}

ATPlaceableActor* ATGridController::SelectObjectAtLocation(const FVector& Location)
{
	int HorizontalPosition = 0;
	int VerticalPosition = 0;
	const bool bIsIncludedInterval = GetPointOnGrid(Location, HorizontalPosition, VerticalPosition);
	if (!bIsIncludedInterval)
	{
		return nullptr;
	}
	return CellObjects[VerticalPosition * HorizontalSize + HorizontalPosition]->SelectActorInCell();
}

bool ATGridController::AnyCanCollectIncome()
{
	bool bAny = false;
	for (UTCellObject* ActorInCell : CellObjects)
	{
		bAny = ActorInCell->GetIsBusy() && ActorInCell->SelectActorInCell()->GetCanCollectIncome();
		if (bAny)
		{
			break;
		}
	}
	return bAny;
}

bool ATGridController::TryRemovePlaceableActorAtLocation(const FVector& Location, EPlaceableObjectType PlaceableObject)
{
	int HorizontalPosition = 0;
	int VerticalPosition = 0;
	const bool bIsIncludedInterval = GetPointOnGrid(Location, HorizontalPosition, VerticalPosition);
	
	bool IsBusyCell = false;
	if (bIsIncludedInterval)
	{
		IsBusyCell = CellObjects[VerticalPosition * HorizontalSize + HorizontalPosition]->GetIsBusy();
	}
	
	if (IsBusyCell && CellObjects[VerticalPosition * HorizontalSize + HorizontalPosition]->GetObjectType() == PlaceableObject)
	{
		CellObjects[VerticalPosition * HorizontalSize + HorizontalPosition]->SetActorInCell(nullptr);
		Cells[VerticalPosition * HorizontalSize + HorizontalPosition]->SetActorHiddenInGame(false);
	}
	return false;
}

bool ATGridController::PlaceAtLocation(class ATPlaceableActor* BuildingActor, const FVector& Location)
{
	if (BuildingActor == nullptr)
	{
		return false;
	}
	int HorizontalPosition = 0;
	int VerticalPosition = 0;
	const bool bIsIncludedInterval = GetPointOnGrid(Location, HorizontalPosition, VerticalPosition);
	if (!bIsIncludedInterval)
	{
		return false;		
	}
	CellObjects[VerticalPosition * HorizontalSize + HorizontalPosition]->SetActorInCell(BuildingActor);
	Cells[VerticalPosition * HorizontalSize + HorizontalPosition]->SetActorHiddenInGame(true);
	return true;
}


bool ATGridController::IsCorrectNeighbour(const int& VerticalPosition, const int& HorizontalPosition, EPlaceableObjectType Type) const
{
	return CellObjects[VerticalPosition * HorizontalSize + HorizontalPosition]->GetObjectType() == Type;
}

bool ATGridController::CheckCorrectNeighbours(EPlaceableObjectType Type, const int& HorizontalPosition,
                                              const int& VerticalPosition) const
{
	switch (Type)
	{		
		case EPlaceableObjectType::Road:
		{
			EPlaceableObjectType CheckType = EPlaceableObjectType::Road;
			const bool CanTopPos = VerticalPosition > 0 && IsCorrectNeighbour(VerticalPosition - 1, HorizontalPosition, CheckType);
			
			const bool CanLeftPos = HorizontalPosition > 0 && IsCorrectNeighbour(VerticalPosition, HorizontalPosition - 1, CheckType);
			
			const bool CanBottomPos = VerticalPosition < VerticalSize - 1 && IsCorrectNeighbour(VerticalPosition + 1, HorizontalPosition, CheckType);
			
			const bool CanRightPos = HorizontalPosition < HorizontalSize - 1 && IsCorrectNeighbour(VerticalPosition, HorizontalPosition + 1, CheckType);

			bool CanBuildRoad = CanTopPos || CanLeftPos || CanBottomPos || CanRightPos;
			if (!CanBuildRoad)
			{
				return CanBuildRoad;
			}
			
			CanBuildRoad = CanBuildRoad && !(CanTopPos && CanLeftPos && IsCorrectNeighbour(VerticalPosition - 1, HorizontalPosition - 1, CheckType));
			CanBuildRoad = CanBuildRoad && !(CanTopPos && CanRightPos && IsCorrectNeighbour(VerticalPosition - 1, HorizontalPosition + 1, CheckType));
			CanBuildRoad = CanBuildRoad && !(CanBottomPos && CanLeftPos && IsCorrectNeighbour(VerticalPosition + 1, HorizontalPosition - 1, CheckType));
			CanBuildRoad = CanBuildRoad && !(CanBottomPos && CanRightPos && IsCorrectNeighbour(VerticalPosition + 1, HorizontalPosition + 1, CheckType));
			return CanBuildRoad;
		}
		case EPlaceableObjectType::Building:
		{
			EPlaceableObjectType CheckType = EPlaceableObjectType::Road;
			const bool CanTopPos = VerticalPosition > 0 && IsCorrectNeighbour(VerticalPosition - 1, HorizontalPosition, CheckType);
			const bool CanLeftPos = HorizontalPosition > 0 && IsCorrectNeighbour(VerticalPosition, HorizontalPosition - 1, CheckType);
			const bool CanBottomPos = VerticalPosition < VerticalSize - 1 && IsCorrectNeighbour(VerticalPosition + 1, HorizontalPosition, CheckType);
			const bool CanRightPos = HorizontalPosition < HorizontalSize - 1 && IsCorrectNeighbour(VerticalPosition, HorizontalPosition + 1, CheckType);
			
			return CanTopPos || CanLeftPos || CanBottomPos || CanRightPos;
		}
		default: break;
	}
	return false;
}
