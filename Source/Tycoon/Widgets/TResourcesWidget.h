// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Tycoon/Controllers/TResourceManager.h"

#include "TResourcesWidget.generated.h"

/**
 * 
 */
UCLASS()
class TYCOON_API UTResourcesWidget : public UUserWidget
{
	GENERATED_BODY()
public:	
	UFUNCTION()
	void OnResourcesValueChanged(const ETResourceType Type, int32 Value);
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Resource", meta=(BindWidget="CurrentWoodText"))
	class UTextBlock* CurrentWoodText;
};
