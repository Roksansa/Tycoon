// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TPlaceableActor.h"

#include "TCellObject.generated.h"

class ATPlaceableActor;
UCLASS()
class TYCOON_API UTCellObject : public UObject
{
	GENERATED_BODY()

public:
	bool GetIsBusy() const;
	void SetActorInCell(ATPlaceableActor* Actor);
	ATPlaceableActor* SelectActorInCell();
	EPlaceableObjectType GetObjectType() const;
protected:
	bool bIsBusy = false;

	UPROPERTY()
	class ATPlaceableActor* PlaceableActor = nullptr;
};
