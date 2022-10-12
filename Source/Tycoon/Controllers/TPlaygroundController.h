// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/TBuildingComponent.h"
#include "GameFramework/Pawn.h"
#include "TPlaygroundController.generated.h"


UCLASS()
class TYCOON_API ATPlaygroundController : public APawn
{
	GENERATED_BODY()

public:
	ATPlaygroundController();
	void BindOnUpdateResourceCount(UObject* Object, FName Name);	
	void BindOnChangeBuildingState(UObject* Object, FName Name);

	virtual void Tick(float DeltaTime) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Zoom(float Value);
	void PlaceNewObject();
	void AddArmPitchInput(float Value);
	UFUNCTION()
	void ChangeBuildingState(ETBuildingMode Mode, ETOnBuildingModeType ModeType, EPlaceableObjectType Type);
	void TryCollectIncome();
	void RemovePlacedObject();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UPawnMovementComponent* MovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Manager, meta = (AllowPrivateAccess = "true"))
	class ATResourceManager* ResourceManager;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GameInfo, meta = (AllowPrivateAccess = "true"))
	class ATGridController* GridController = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameInfo, meta = (AllowPrivateAccess = "true", AllowAbstract = "true"))
	class UTBuildingComponent* BuildingComponent = nullptr;

	void SpawnStartingObject();

	//non-gaming logic
private:
	
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	//temp cursor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MinArmLength = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MaxArmLength = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = 0.f, UIMin = 0.f))
	float ZoomRate = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = 0.f, UIMin = 0.f))
	float LookAtRate = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float StartArmRotationPitch = -60.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MinEndArmRotationPitch = -60.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MaxEndArmRotationPitch = -30.f;
};
