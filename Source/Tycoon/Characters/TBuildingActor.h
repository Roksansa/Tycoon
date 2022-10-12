// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TPlaceableActor.h"
#include "TBuildingActor.generated.h"


UCLASS(Blueprintable)
class TYCOON_API ATBuildingActor : public ATPlaceableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATBuildingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
