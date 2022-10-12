// Fill out your copyright notice in the Description page of Project Settings.


#include "TCellObject.h"
#include "TPlaceableActor.h"


bool UTCellObject::GetIsBusy() const
{
	return bIsBusy;
}

void UTCellObject::SetActorInCell(ATPlaceableActor* Actor)
{
	if (PlaceableActor != nullptr)
	{
		PlaceableActor->Destroy();
		//TODO need clear
	}
	PlaceableActor = Actor;
	bIsBusy = PlaceableActor != nullptr;
	if (!bIsBusy)
	{
		return;
	}
	const FVector Location = PlaceableActor->GetActorLocation();
	PlaceableActor->SetActorLocation(FVector(Location.X, Location.Y, -1.f));
}

ATPlaceableActor* UTCellObject::SelectActorInCell()
{
	return PlaceableActor;
}

EPlaceableObjectType UTCellObject::GetObjectType() const
{
	return bIsBusy? PlaceableActor->GetObjectType() : EPlaceableObjectType::None;
}
