// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Hexagon.h"
#include "GameFramework/PlayerController.h"
#include "PathfinderPlayerController.generated.h"

UCLASS()
class APathfinderPlayerController : public APlayerController
{
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere)
	//class UPathfinderMovementComponent* MovementController;

	class ASimsAIController* AiController;

public:
	APathfinderPlayerController();
	//void SetNewDestination(class AHexagon* hex);

protected:

	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
};


