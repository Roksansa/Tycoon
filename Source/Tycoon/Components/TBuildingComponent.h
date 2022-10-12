// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/TPlaceableActor.h"
#include "Components/ActorComponent.h"

#include "TBuildingComponent.generated.h"

UENUM(BlueprintType)
enum class ETBuildingMode : uint8
{
	None = 0,
	On,
	Off,
	RepeatedOn,
	PlacedToOff,
	MAX UMETA(Hidden)
};

//only for (ETBuildingMode equal On) correct
UENUM(BlueprintType)
enum class ETOnBuildingModeType : uint8
{
	None = 0,
	Create,
	Destroy,
	MAX UMETA(Hidden)
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeBuildingState, ETBuildingMode);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TYCOON_API UTBuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTBuildingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	ATPlaceableActor* GetSpawnedObject();
	void UpdatePositionSpawnedObject(const FVector& LocationAtGrid, bool bCanPlaceable);
	bool NeedPositionObjectDraw() const;
	void ChangeBuildingStateToOff();
	void ChangeBuildingState(const FVector& LocationAtGrid, ETBuildingMode Mode, ETOnBuildingModeType ModeType, EPlaceableObjectType Type);
	EPlaceableObjectType GetPlaceableObjectType() const;
	FOnChangeBuildingState OnChangeBuildingState;
	ETOnBuildingModeType GetBuildingModeType() const;
private:
	void JustClearSpawnedObject();
	bool SpawnObject(const FVector& LocationAtGrid, EPlaceableObjectType Type);
	
	ETBuildingMode BuildingMode = ETBuildingMode::Off;
	ETOnBuildingModeType BuildingModeType = ETOnBuildingModeType::None;
	EPlaceableObjectType DestroyType = EPlaceableObjectType::None;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GameInfo, meta = (AllowPrivateAccess = "true"))
	class ATPlaceableActor* CurrentBuildingPressed = nullptr;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameInfo, meta = (AllowPrivateAccess = "true", AllowAbstract = "true"))
	TMap<EPlaceableObjectType, TSubclassOf<class ATPlaceableActor>> TemplateObjects;
};
