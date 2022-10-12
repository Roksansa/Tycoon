// Fill out your copyright notice in the Description page of Project Settings.


#include "TMainHUDWidget.h"

#include "TResourcesWidget.h"
#include "TShopWidget.h"
#include "Blueprint/WidgetTree.h"

UTResourcesWidget* UTMainHUDWidget::GetResourcesWidget()
{
	return WidgetTree->FindWidget<UTResourcesWidget>(ResourcesWidgetName);
}

UTShopWidget* UTMainHUDWidget::GetShopWidget()
{
	return WidgetTree->FindWidget<UTShopWidget>(ShopWidgetName);
}
