// Fill out your copyright notice in the Description page of Project Settings.


#include "TResourcesWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

void UTResourcesWidget::OnResourcesValueChanged(const ETResourceType Type, int32 Value)
{
	switch (Type)
	{
		case ETResourceType::Wood:
		{
			CurrentWoodText->SetText(UKismetTextLibrary::Conv_IntToText(Value));
			break;
		}
		default: break;
	}
}
