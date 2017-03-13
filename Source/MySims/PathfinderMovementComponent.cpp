// Fill out your copyright notice in the Description page of Project Settings.

#include "Pathfinder.h"
#include "PathfinderMovementComponent.h"
#include "Hexagon.h"
#include <unordered_map>
#include "Pathfinding.h"
#include "Kismet/KismetMathLibrary.h"


UPathfinderMovementComponent::UPathfinderMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPathfinderMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPathfinderMovementComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (m_currentPath.size() > 0)
	{
		recalculatePath(); 
		performMove(DeltaTime);
	}
	else if(m_currentPath.size() == 0 && m_repeatPath && m_repeatPathStartHexagon && m_currentHexagon)
	{
		Move(m_currentHexagon, m_repeatPathStartHexagon, true);
	}
}

void UPathfinderMovementComponent::MoveInstantly(AHexagon* destination)
{
	m_currentHexagon = destination;
	moveActor(destination, destination, 1.0f);
}

void UPathfinderMovementComponent::Move(AHexagon* from, AHexagon* to, bool repeat)
{
	move(from, to, true);
	m_repeatPath = repeat;
	if(repeat)
	{
		m_repeatPathStartHexagon = from;
	}
}

void UPathfinderMovementComponent::MoveFromCurrentPositionTo(AHexagon* destination)
{
	Move(m_currentHexagon, destination);
}

void UPathfinderMovementComponent::moveActor(AHexagon* from, AHexagon* to, float lerpAlpha)
{
	FVector new_location = FMath::Lerp(from->GetActorLocation(), to->GetActorLocation(), lerpAlpha);
	new_location.Z = ActorToMove->GetActorLocation().Z;

	if(bRotateActor)
	{
		FRotator new_rotation = UKismetMathLibrary::FindLookAtRotation(ActorToMove->GetActorLocation(), FVector(new_location.X, new_location.Y, ActorToMove->GetActorLocation().Z));
		ActorToMove->SetActorLocationAndRotation(new_location, new_rotation);
	}
	else
		ActorToMove->SetActorLocation(new_location);
}

void UPathfinderMovementComponent::move(AHexagon* from, AHexagon* to, bool resetPerformMoveParameter)
{
	std::unordered_map<AHexagon*, AHexagon*> came_from;
	std::unordered_map<AHexagon*, float> cost_so_far;
	Pathfinding::A_StarSearch(from, to, came_from, cost_so_far, bConsiderThreat);

	//reset current path
	if (bVisualizePath)
	{
		if (m_destinationHexagon)
			m_destinationHexagon->ActivateBlinking(false, resetPerformMoveParameter);
		for (auto a : m_currentPath)
			a->SetTerrainColor();
	}

	m_currentPath.clear();
	m_currentPath = Pathfinding::ReconstructPath(from, to, came_from);
	m_destinationHexagon = to;
	if(resetPerformMoveParameter)
		m_currentMoveTime = 0;

	//show new path
	if (bVisualizePath)
	{
		for (auto a : m_currentPath)
			a->SetPathColor(true);

		from->SetTerrainColor();
		m_destinationHexagon->SetDestinationColor();
		m_destinationHexagon->ActivateBlinking(true, resetPerformMoveParameter);
	}

	//delete start from path for correct moving
	m_currentPath.pop_back();
}

void UPathfinderMovementComponent::recalculatePath()
{
	if (!bConsiderThreat) return;
	//just if threat is on one of the path hexagons!!
	bool threatOnPath = false;
	for(auto a : m_currentPath)
	{
		threatOnPath = a->IsThreat();
		if (threatOnPath)
			break;
	}
	if(threatOnPath)
		move(m_currentHexagon, m_destinationHexagon, false);
}

void UPathfinderMovementComponent::performMove(float deltaTime)
{
	m_currentMoveTime += deltaTime;

	if (m_currentMoveTime > 0 && m_currentMoveTime <= OneMoveTime && bLerpMovement)
		moveActor(m_currentHexagon, m_currentPath.back(), m_currentMoveTime / OneMoveTime);
	else if (m_currentMoveTime > OneMoveTime)
	{ 
		if (OnBeforeReachDestination != nullptr)
			OnBeforeReachDestination();
		if(!bLerpMovement)
			moveActor(m_currentHexagon, m_currentPath.back(), 1);
		m_currentHexagon = m_currentPath.back();
		if(bVisualizePath)
			m_currentHexagon->SetTerrainColor();
		m_currentPath.pop_back();
		m_currentMoveTime = 0;

		if (m_currentHexagon == m_destinationHexagon)
			m_destinationHexagon->ActivateBlinking(false);
	}
}

void UPathfinderMovementComponent::IncreaseOneMoveTime()
{
	OneMoveTime += m_oneMoveTimeInDecreaseDelta;
}

void UPathfinderMovementComponent::DecreaseOneMoveTime()
{
	if(OneMoveTime - m_oneMoveTimeInDecreaseDelta > 0.0f)
		OneMoveTime -= m_oneMoveTimeInDecreaseDelta;
}
