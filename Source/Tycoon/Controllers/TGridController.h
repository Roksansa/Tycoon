// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TBuildingComponent.h"
#include "GameFramework/Actor.h"

#include "TGridController.generated.h"

enum class EPlaceableObjectType : uint8;

DECLARE_DELEGATE(FOnEndedBeginPlay);
UCLASS()
class TYCOON_API ATGridController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATGridController();

	const FVector GetLocationOnGrid(const FVector& Location) const;
	bool GetPointOnGrid(const FVector& Location, int& HorizontalPosition, int& VerticalPosition) const;
	bool PlaceAtLocation(class ATPlaceableActor* BuildingActor, const FVector& Location);
	bool IsCorrectNeighbour(const int& VerticalPosition, const int& HorizontalPosition, EPlaceableObjectType Type) const;
	bool CheckCorrectNeighbours(EPlaceableObjectType Type, const int& HorizontalPosition, const int& VerticalPosition) const;
	bool CanPlaceAtLocation(EPlaceableObjectType Type, const FVector& Location) const;
	const FVector GetCenterOnGrid() const;
	ATPlaceableActor* SelectObjectAtLocation(const FVector& Location);
	bool AnyCanCollectIncome();
	bool TryRemovePlaceableActorAtLocation(const FVector& Location, EPlaceableObjectType PlaceableObject);
	FOnEndedBeginPlay OnEndedBeginPlay;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = Info)
	TArray<AActor*> Cells;

	UFUNCTION(BlueprintImplementableEvent)
	void InitAfterBeginPlay();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info|Grid", meta = (AllowPrivateAccess = "true", UIMin = 1, ClampMin = 1))
	int HorizontalSize = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info|Grid", meta = (AllowPrivateAccess = "true", UIMin = 1, ClampMin = 1))
	int VerticalSize = 1;
	/** Cell size in sm*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info|Grid", meta = (AllowPrivateAccess = "true", UIMin = 1, ClampMin = 1))
	int CellSize = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info|Grid", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> TemplateSpawnActor;

	UPROPERTY()
	TArray<class UTCellObject*> CellObjects;

	FVector TopLeftCornerLocation = FVector::ZeroVector;
};
