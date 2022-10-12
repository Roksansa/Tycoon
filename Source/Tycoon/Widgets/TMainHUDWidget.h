// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "TMainHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class TYCOON_API UTMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	class UTResourcesWidget* GetResourcesWidget();
	class UTShopWidget* GetShopWidget();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName ResourcesWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetName")
	FName ShopWidgetName;
};
