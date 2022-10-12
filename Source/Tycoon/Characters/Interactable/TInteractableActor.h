// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TInteractableActor.generated.h"

UCLASS()
class TYCOON_API ATInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATInteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//set value in blueprint construction script
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Config")
	class UBoxComponent* BoxCollider;
};
