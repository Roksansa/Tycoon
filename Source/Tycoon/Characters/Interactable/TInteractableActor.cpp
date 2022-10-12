// Fill out your copyright notice in the Description page of Project Settings.


#include "TInteractableActor.h"

#include "Components/BoxComponent.h"

// Sets default values
ATInteractableActor::ATInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	RootComponent = BoxCollider;
}

// Called when the game starts or when spawned
void ATInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(BoxCollider))
	{
		BoxCollider->SetGenerateOverlapEvents(true);
		BoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		BoxCollider->bHiddenInGame = false;
	}
}

