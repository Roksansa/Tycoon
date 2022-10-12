// Fill out your copyright notice in the Description page of Project Settings.


#include "TPlaygroundController.h"

#include "TGridController.h"
#include "TPlayerController.h"
#include "TResourceManager.h"
#include "Camera/CameraComponent.h"
#include "Characters/TPlaceableActor.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tycoon/Components/TBuildingComponent.h"
#include "Utils/TConstants.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlaygroundController, Verbose, Verbose);

// Sets default values
ATPlaygroundController::ATPlaygroundController()
{
	RootComponent = CreateDefaultSubobject<UBoxComponent>("Root");

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	CursorToWorld->DecalSize = FVector(0.01f, 16.0f, 16.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);
	MovementComponent->bConstrainToPlane = true;
	MovementComponent->bSnapToPlaneAtStart = true;

	BuildingComponent = CreateDefaultSubobject<UTBuildingComponent>(TEXT("BuildingComponent"));

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATPlaygroundController::BindOnUpdateResourceCount(UObject* Object, FName Name)
{
	if (!IsValid(Object))
	{
		return;
	}
	ResourceManager->OnUpdateResourceCount.AddUFunction(Object, Name);
	ResourceManager->CheckResources();
}

void ATPlaygroundController::BindOnChangeBuildingState(UObject* Object, FName Name)
{
	if (!IsValid(Object))
	{
		return;
	}
	BuildingComponent->OnChangeBuildingState.AddUFunction(Object, Name);
}

void ATPlaygroundController::ChangeBuildingState(ETBuildingMode Mode, ETOnBuildingModeType ModeType, EPlaceableObjectType Type)
{
	if (GridController == nullptr || CursorToWorld == nullptr)
	{
		return;
	}
	const FVector Location = CursorToWorld->GetComponentLocation();
	BuildingComponent->ChangeBuildingState(GridController->GetLocationOnGrid(Location), Mode, ModeType, Type);
}

void ATPlaygroundController::TryCollectIncome()
{
	if (!IsValid(ResourceManager) || !IsValid(GridController))
	{
		return;
	}
	if (!GridController->AnyCanCollectIncome())
	{
		return;
	}
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult TraceHitResult;
		PC->GetHitResultUnderCursor(TConstants::ECC_Ground, false, TraceHitResult);
		if (!TraceHitResult.bBlockingHit)
		{
			return;
		}
		
		ATPlaceableActor* CurrentObject = GridController->SelectObjectAtLocation(TraceHitResult.Location);
		int32 CountReward = 0;
		if (CurrentObject && CurrentObject->CollectedIncome(CountReward))
		{
			ResourceManager->Add(CurrentObject->GetIncome().GetResourceType(), CountReward);
		}
	}
}

// Called when the game starts or when spawned
void ATPlaygroundController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATResourceManager::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		ResourceManager = StaticCast<ATResourceManager*>(FoundActors[0]);
	}
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATGridController::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		GridController = StaticCast<ATGridController*>(FoundActors[0]);
	}
	checkf(GridController != nullptr, TEXT("ATPlaygroundController::BeginPlay Need GridController"));
	checkf(ResourceManager != nullptr, TEXT("ATPlaygroundController::BeginPlay Need ResourceManager"));
	
	if (GridController->HasActorBegunPlay())
	{
		FString GetLocationOnGridLog = FString::Format(TEXT("IN Spawn "), {TEXT("AT")});
		UE_LOG(LogEngine, Error, TEXT("%s 0"), *GetLocationOnGridLog);
		SpawnStartingObject();
	}
	else
	{
		GridController->OnEndedBeginPlay.BindUFunction(this, FName("SpawnStartingObject"));
	}

	ATPlayerController* CurrentPlayerController = Cast<ATPlayerController>(GetController());
	if (CurrentPlayerController != nullptr)
	{
		CurrentPlayerController->BindWidgets();
	}
}


// Called every frame
void ATPlaygroundController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CursorToWorld != nullptr)
	{		
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult);
			CursorToWorld->SetVisibility(TraceHitResult.bBlockingHit);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			FVector NewLocation = TraceHitResult.Location;
			CursorToWorld->SetWorldLocation(NewLocation);
			CursorToWorld->SetWorldRotation(CursorR);
			if(TraceHitResult.bBlockingHit && IsValid(GridController) && BuildingComponent->NeedPositionObjectDraw())
			{
				const TArray<FTResourceDesc>& Price = BuildingComponent->GetSpawnedObject()->GetPrice();
				bool bCanBuyThisObject = ResourceManager->CanBuy(Price);
				const bool bCanPlaceable = bCanBuyThisObject && GridController->CanPlaceAtLocation(BuildingComponent->GetPlaceableObjectType(), NewLocation);
				BuildingComponent->UpdatePositionSpawnedObject(GridController->GetLocationOnGrid(NewLocation), bCanPlaceable);				
			}
		}
	}
}

void ATPlaygroundController::MoveForward(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1E-06f))
	{
		const FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		const FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void ATPlaygroundController::MoveRight(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1E-06f))
	{
		const FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		const FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void ATPlaygroundController::Zoom(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1E-06f))
	{
		CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength + Value * ZoomRate, MinArmLength, MaxArmLength);

		float CurrentPitch = CameraBoom->GetRelativeRotation().Pitch;
		float PercentOfFull = (CameraBoom->TargetArmLength - MinArmLength) / (MaxArmLength - MinArmLength);		
		float NewPitch = PercentOfFull*(MinEndArmRotationPitch - MaxEndArmRotationPitch) + MaxEndArmRotationPitch;
		float DeltaPitch = 5.f;
		if (FMath::IsWithin(CurrentPitch, NewPitch - DeltaPitch,NewPitch))
		{
			CurrentPitch = NewPitch;
		}
		CurrentPitch = FMath::Clamp(CurrentPitch, StartArmRotationPitch, NewPitch);
		CameraBoom->SetRelativeRotation(FRotator(CurrentPitch, 0, 0.f));
	}
}

void ATPlaygroundController::AddArmPitchInput(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1E-06f))
	{
		float CurrentPitch = CameraBoom->GetRelativeRotation().Pitch;
		float PercentOfFull = (CameraBoom->TargetArmLength - MinArmLength) / (MaxArmLength - MinArmLength);		
		float NewPitch = PercentOfFull*(MinEndArmRotationPitch - MaxEndArmRotationPitch) + MaxEndArmRotationPitch;
		float AddNew = Value * LookAtRate;
		CurrentPitch = FMath::Clamp(CurrentPitch + AddNew, StartArmRotationPitch, NewPitch);
		CameraBoom->SetRelativeRotation(FRotator(CurrentPitch, 0, 0.f));
	}	
}

void ATPlaygroundController::PlaceNewObject()
{
	if (CursorToWorld ==  nullptr || GridController == nullptr || BuildingComponent->GetBuildingModeType() != ETOnBuildingModeType::Create)
	{
		return;
	}
	ATPlaceableActor* ObjectForPlace = BuildingComponent->GetSpawnedObject();

	if (ObjectForPlace == nullptr)
	{
		return;
	}

	const TArray<FTResourceDesc>& Price = ObjectForPlace->GetPrice();
	bool bCanBuyThisObject = ResourceManager->CanBuy(Price);

	FVector Location = CursorToWorld->GetComponentLocation();
	if (bCanBuyThisObject && GridController->CanPlaceAtLocation(ObjectForPlace->GetObjectType(), Location))
	{
		bool Placed = GridController->PlaceAtLocation(ObjectForPlace, Location);
		bool bCanBuyRepeatObject = false;
		if (Placed)
		{
			ResourceManager->Spend(Price);
			bCanBuyRepeatObject = ResourceManager->CanBuy(Price);
			ObjectForPlace->StartBuilding();
		}
		BuildingComponent->ChangeBuildingState(GridController->GetLocationOnGrid(Location),
		                                       Placed && bCanBuyRepeatObject ? ETBuildingMode::RepeatedOn : ETBuildingMode::PlacedToOff,
		                                       ETOnBuildingModeType::None,
		                                       EPlaceableObjectType::None);
	}
}

void ATPlaygroundController::RemovePlacedObject()
{
	if (CursorToWorld ==  nullptr || GridController == nullptr || BuildingComponent->GetBuildingModeType() != ETOnBuildingModeType::Destroy)
	{
		return;
	}


	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult TraceHitResult;
		PC->GetHitResultUnderCursor(TConstants::ECC_Ground, false, TraceHitResult);
		if (!TraceHitResult.bBlockingHit)
		{
			return;
		}
		
		bool bTry = GridController->TryRemovePlaceableActorAtLocation(TraceHitResult.Location, BuildingComponent->GetPlaceableObjectType());
		BuildingComponent->ChangeBuildingState(GridController->GetLocationOnGrid(TraceHitResult.Location),
										   ETBuildingMode::RepeatedOn,
										   ETOnBuildingModeType::Destroy,
										   BuildingComponent->GetPlaceableObjectType());
	}
}

void ATPlaygroundController::SpawnStartingObject()
{
	FString GetLocationOnGridLog = FString::Format(TEXT("IN SpawnStartingObject "), {TEXT("AT")});
	UE_LOG(LogEngine, Error, TEXT("%s 0"), *GetLocationOnGridLog);

	ChangeBuildingState(ETBuildingMode::On, ETOnBuildingModeType::Create, EPlaceableObjectType::Road);
	
	if (GridController == nullptr)
	{
		return;
	}

	ATPlaceableActor* ObjectForPlace = BuildingComponent->GetSpawnedObject();

	if (ObjectForPlace == nullptr)
	{
		return;
	}

	FVector Location = GridController->GetCenterOnGrid();
	bool Placed = GridController->PlaceAtLocation(ObjectForPlace, Location);
	BuildingComponent->ChangeBuildingState(GridController->GetLocationOnGrid(Location), 
											Placed ? ETBuildingMode::PlacedToOff : ETBuildingMode::On,
											ETOnBuildingModeType::None,
											EPlaceableObjectType::None);
}
