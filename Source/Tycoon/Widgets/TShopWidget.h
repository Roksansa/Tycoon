// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/TPlaceableActor.h"
#include "Components/Image.h"
#include "Components/TBuildingComponent.h"
#include "Controllers/TPlaygroundController.h"

#include "TShopWidget.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnNeedChangeBuildingState, ETBuildingMode, ETOnBuildingModeType, EPlaceableObjectType);

USTRUCT(BlueprintType)
struct FShopButtonDesc
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building")
	class UImage* ImageWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building")
	ETBuildingMode Mode = ETBuildingMode::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building", meta = (EditCondition = "Mode == ETBuildingMode::On"))
	ETOnBuildingModeType ModeType = ETOnBuildingModeType::None;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building", meta = (EditCondition = "Mode == ETBuildingMode::On"))
	EPlaceableObjectType ObjectType = EPlaceableObjectType::None;
};

UCLASS()
class TYCOON_API UTShopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void BindOnNeedChangeBuildingState(UObject* Object, const FName& Name);
protected:
	
	UFUNCTION()
	void OnMouseClicked(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION()
	void OnChangeBuildingState(ETBuildingMode Mode);
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building")
	float ImageXSize = 128.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building")
	class UImage* BorderHighlight;
	
	TArray<SWidget*> CachedImageSWidgets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Building")
	TArray<FShopButtonDesc> ShopButtonDescs;

private:
	int CurrentPressedButton = -1;
	
	FOnNeedChangeBuildingState OnNeedChangeBuildingState;
};
