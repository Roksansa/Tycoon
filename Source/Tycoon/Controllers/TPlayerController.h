// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "Widgets/TShopWidget.h"

#include "TPlayerController.generated.h"

/**
 * Замены Pawn не будет, поэтому весь импут реализован непосредственно в классе PlaygroundController
 */
UCLASS()
class TYCOON_API ATPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn* InPawn) override;
	void BindWidgets();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget")
	TSubclassOf<class UTMainHUDWidget> MainWidgetTemplate;

	//*Key if pressed this key - enable TurnAt and LookUp*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setting")
	FKey KeyChangedRotation;
	virtual void SetupInputComponent() override;

private:
	void InitWidgets();
	bool bIsWidgetsBinded = false;
	bool IsWidgetDirectlyUnderCursor(UTShopWidget* Widget) const;
	
	TWeakObjectPtr<class UTMainHUDWidget> MainWidget;	
	TWeakObjectPtr<class ATPlaygroundController> CachedPlaygroundController;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Zoom(float Value);
	void TurnAt(float Value);
	void LookUp(float Value);
	void PlaceNewObject();
	void TryCollectIncome();
	void RemovePlacedObject();
};
