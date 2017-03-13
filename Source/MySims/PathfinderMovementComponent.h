// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <vector>
#include "Components/ActorComponent.h"
#include "PathfinderMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYSIMS_API UPathfinderMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPathfinderMovementComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void MoveInstantly(class AHexagon* destination);
	void Move(class AHexagon* from, class AHexagon* to, bool repeat = false);
	void MoveFromCurrentPositionTo(class AHexagon* destination);

	void IncreaseOneMoveTime();
	void DecreaseOneMoveTime();


	AActor* ActorToMove;

	UPROPERTY(EditDefaultsOnly)
		bool bVisualizePath = false;
	UPROPERTY(EditAnywhere)
		float OneMoveTime = 0.5f;
	UPROPERTY(EditAnywhere)
		bool bConsiderThreat = true;
	UPROPERTY(EditAnywhere)
		bool bLerpMovement = true;
	UPROPERTY(EditAnywhere)
		bool bRotateActor = true;

	std::function<void(void)> OnBeforeReachDestination = nullptr;

private:
	class AHexagon* m_destinationHexagon;
	class AHexagon* m_currentHexagon;
	float m_currentMoveTime;
	std::vector<class AHexagon*> m_currentPath;
	float m_oneMoveTimeInDecreaseDelta = 0.05f;
	bool m_repeatPath = false;
	class AHexagon* m_repeatPathDestinationHexagon;
	class AHexagon* m_repeatPathStartHexagon;

	void moveActor(class AHexagon* from, class AHexagon* to, float lerpAlpha);
	void move(class AHexagon* from, class AHexagon* to, bool resetPerformMoveParameter = true);
	void recalculatePath();
	void performMove(float deltaTime);
};
