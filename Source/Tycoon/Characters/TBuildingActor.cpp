// Fill out your copyright notice in the Description page of Project Settings.


#include "TBuildingActor.h"

ATBuildingActor::ATBuildingActor()
{
	PlaceableObjectType = EPlaceableObjectType::Building;
}

void ATBuildingActor::BeginPlay()
{
	Super::BeginPlay();
}
