// Fill out your copyright notice in the Description page of Project Settings.


#include "TShopWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/PanelSlot.h"

void UTShopWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	for (int i = 0; i < ShopButtonDescs.Num(); i++)
	{
		ShopButtonDescs[i].ImageWidget->OnMouseButtonDownEvent.BindUFunction(this, "OnMouseClicked");
	}	
}

void UTShopWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CachedImageSWidgets.Empty();
	CachedImageSWidgets.Reserve(4);
	for (int i = 0; i < ShopButtonDescs.Num(); i++)
	{
		CachedImageSWidgets.Add(&ShopButtonDescs[i].ImageWidget->TakeWidget().Get());
	}
	BorderHighlight->SetVisibility(ESlateVisibility::Hidden);
	CurrentPressedButton = -1;
}

void UTShopWidget::BindOnNeedChangeBuildingState(UObject* Object, const FName& Name)
{
	if (!IsValid(Object))
	{
		return;
	}
	OnNeedChangeBuildingState.AddUFunction(Object, Name);
}

void UTShopWidget::OnMouseClicked(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Format(TEXT(" Clicked  Mouse {0}"), {MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)}), true);
	if (!MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return;
	}

	int ButtonNumber = -1;
	const SWidget* ClickedWidget = &MouseEvent.GetEventPath()->GetLastWidget().Get();

	for (int i = 0; i < CachedImageSWidgets.Num() && ButtonNumber == -1; i++)
	{
		ButtonNumber = ClickedWidget == CachedImageSWidgets[i] ? i : -1;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Format(TEXT(" Clicked  Mouse22 {0} {1} -- {2}"), {MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton), ButtonNumber, CurrentPressedButton != ButtonNumber}), true);

	if (ButtonNumber == -1)
	{
		return;
	}

	const bool bNeedHighlight = CurrentPressedButton != ButtonNumber;
	CurrentPressedButton = bNeedHighlight ? ButtonNumber : -1;
	BorderHighlight->SetVisibility(bNeedHighlight ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	FShopButtonDesc Desc {nullptr, ETBuildingMode::None, ETOnBuildingModeType::None, EPlaceableObjectType::None};
	if (bNeedHighlight)
	{
		UCanvasPanelSlot* GridAsPanelSlot = Cast<UCanvasPanelSlot>(BorderHighlight->Slot);
		GridAsPanelSlot->SetPosition(FVector2D(ImageXSize * ButtonNumber, 0.f));
		Desc = ShopButtonDescs[ButtonNumber];
	}
	OnNeedChangeBuildingState.Broadcast(Desc.Mode, Desc.ModeType, Desc.ObjectType);
}

void UTShopWidget::OnChangeBuildingState(ETBuildingMode Mode)
{
	if (Mode == ETBuildingMode::Off && CurrentPressedButton != -1)
	{
		CurrentPressedButton = -1;
		BorderHighlight->SetVisibility(ESlateVisibility::Hidden);
	}
}
