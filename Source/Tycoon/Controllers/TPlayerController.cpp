// Fill out your copyright notice in the Description page of Project Settings.


#include "TPlayerController.h"


#include "TPlaygroundController.h"
#include "TResourceManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Tycoon/Widgets/TMainHUDWidget.h"
#include "Tycoon/Widgets/TResourcesWidget.h" // need for bing delegate - don't delete
#include "Utils/TConstants.h"
#include "Widgets/TShopWidget.h"

void ATPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedPlaygroundController = Cast<ATPlaygroundController>(InPawn);
	if (CachedPlaygroundController.IsValid())
	{
		InitWidgets();
	}
}

void ATPlayerController::InitWidgets()
{
	if ((MainWidgetTemplate != nullptr && MainWidget == nullptr))
	{
		MainWidget = CreateWidget<UTMainHUDWidget>(GetWorld(), MainWidgetTemplate.Get());
		if (!MainWidget.IsValid())
		{
			return;
		}
		MainWidget->AddToViewport();
	}
}

void ATPlayerController::BindWidgets()
{
	if (MainWidget == nullptr || !CachedPlaygroundController.IsValid() || !CachedPlaygroundController->HasActorBegunPlay())
	{
		return;
	}

	if (bIsWidgetsBinded)
	{
		return;
	}
	UTResourcesWidget* ResourcesWidget = MainWidget->GetResourcesWidget();
	CachedPlaygroundController->BindOnUpdateResourceCount(ResourcesWidget, TConstants::OnResourcesValueChangedName);
	
	UTShopWidget* ShopWidget = MainWidget->GetShopWidget();
	if (!IsValid(ShopWidget))
	{
		return;
	}
	ShopWidget->BindOnNeedChangeBuildingState(CachedPlaygroundController.Get(), TConstants::ChangeBuildingStateName);
	CachedPlaygroundController->BindOnChangeBuildingState(ShopWidget, TConstants::OnChangeBuildingStateName);

	bIsWidgetsBinded = true;
}

void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("Turn", this, &ATPlayerController::TurnAt);
	InputComponent->BindAxis("LookUp", this, &ATPlayerController::LookUp);
	InputComponent->BindAxis("MoveForward", this, &ATPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATPlayerController::MoveRight);
	InputComponent->BindAxis("ZoomUp", this, &ATPlayerController::Zoom);
	InputComponent->BindAction("PlaceNewObject", EInputEvent::IE_Released, this, &ATPlayerController::PlaceNewObject);
	InputComponent->BindAction("CollectIncome", EInputEvent::IE_Released, this, &ATPlayerController::TryCollectIncome);
	InputComponent->BindAction("RemovePlacedObject", EInputEvent::IE_Released, this, &ATPlayerController::RemovePlacedObject);
}

void ATPlayerController::MoveForward(float Value)
{
	if (CachedPlaygroundController.IsValid())
	{
		CachedPlaygroundController->MoveForward(Value);
	}
}

void ATPlayerController::MoveRight(float Value)
{
	if (CachedPlaygroundController.IsValid())
	{
		CachedPlaygroundController->MoveRight(Value);
	}
}

void ATPlayerController::Zoom(float Value)
{
	if (CachedPlaygroundController.IsValid())
	{
		CachedPlaygroundController->Zoom(Value);
	}
}

void ATPlayerController::PlaceNewObject()
{
	if (CachedPlaygroundController.IsValid() && !IsWidgetDirectlyUnderCursor(MainWidget->GetShopWidget()))
	{
		CachedPlaygroundController->PlaceNewObject();
	}
}

void ATPlayerController::TryCollectIncome()
{
	if (CachedPlaygroundController.IsValid() && !IsWidgetDirectlyUnderCursor(MainWidget->GetShopWidget()))
	{
		CachedPlaygroundController->TryCollectIncome();
	}
}

void ATPlayerController::RemovePlacedObject()
{
	if (CachedPlaygroundController.IsValid() && !IsWidgetDirectlyUnderCursor(MainWidget->GetShopWidget()))
	{
		CachedPlaygroundController->RemovePlacedObject();
	}
}

void ATPlayerController::TurnAt(float Value)
{
	if (CachedPlaygroundController.IsValid() && IsInputKeyDown(KeyChangedRotation))
	{
		CachedPlaygroundController->AddControllerYawInput(Value);
	}
}

void ATPlayerController::LookUp(float Value)
{
	if (CachedPlaygroundController.IsValid() && IsInputKeyDown(KeyChangedRotation))
	{
		CachedPlaygroundController->AddArmPitchInput(Value);
	}
}

bool ATPlayerController::IsWidgetDirectlyUnderCursor(UTShopWidget* Widget) const
{
	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer && IsValid(Widget))
	{
		TSharedPtr<const FSlateUser> SlateUser = LocalPlayer->GetSlateUser();

		FSlateApplication& SlateApp = FSlateApplication::Get();
		FWidgetPath UnderCursorWidgets = SlateApp.LocateWindowUnderMouse(SlateApp.GetCursorPos(), SlateApp.GetInteractiveTopLevelWindows(),
			false, SlateUser->GetUserIndex());

		const int32 Count = UnderCursorWidgets.Widgets.Num();
		for (int32 Index = 0; Index < Count; Index++)
		{
			TSharedRef<SWidget> FoundWidget = UnderCursorWidgets.Widgets[Index].Widget;
			if (FoundWidget == Widget->TakeWidget())
			{
				return true;
			}
		}
	}
	return false;
}
