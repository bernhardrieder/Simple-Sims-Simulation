// Fill out your copyright notice in the Description page of Project Settings.

#include "Pathfinder.h"
#include "SimsAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EngineUtils.h"
#include <unordered_map>
#include "Pathfinding.h"
#include "PathfinderCharacter.h"

void ASimsAIController::BeginPlay()
{
	Super::BeginPlay();

	//find ai character -> automatic setting not possible due to 2 controller??
	for (TActorIterator<APathfinderCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		m_aiCharacter = *ActorItr;
		if (m_aiCharacter)
			break;
	}
	if (!m_aiCharacter)
		UE_LOG(LogPathfinder, Error, TEXT("Can't find Ai Character!"));

	m_aiCharacter->SetAiController(this);

	//init all possible ai interaction objects
	for (auto object : MySimsObjects)
		m_possibleDestinations.Add(object, TArray<AHexagon*>());
	//find every hexagon according to the object type
	bool startFound = false;
	for (TActorIterator<AHexagon> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AHexagon* hex = *ActorItr;

		if (!startFound && hex->ActorHasTag("START") && hex)
		{
			moveActorTo(hex);
			startFound = true;
		}

		for (auto object : MySimsObjects)
			if (hex->GetTerrainType() == object)
				m_possibleDestinations.Find(object)->Add(hex);
	}
	if (!startFound)
		UE_LOG(LogPathfinder, Error, TEXT("NO STARTPOINT FOUND!! Define a start Hexagon with Actortag = START"));


	if (UseBlackboard(BlackboardData, Blackboard))
	{
		Blackboard->SetValueAsBool("true", true);
		Blackboard->SetValueAsBool("false", false);
		if(!RunBehaviorTree(BehaviorTree))
			UE_LOG(LogPathfinder, Error, TEXT("Not able to run Behaviortree!"));
	}
	else
		UE_LOG(LogPathfinder, Error, TEXT("Not able to set Blackboard!"));
}

APathfinderCharacter* ASimsAIController::GetAiCharacter() const
{
	return m_aiCharacter;
}

bool ASimsAIController::MoveTo(ETerrainType type)
{
	m_finishedMove = false;
	return initMove(type);
}

void ASimsAIController::KeepMoving(float deltaTime)
{
	if (m_currentPath.size() > 0)
		performMove(deltaTime);
	if (m_currentPath.size() == 0)
		m_finishedMove = true;
}

bool ASimsAIController::FinishedMoving() const
{
	return m_finishedMove;
}

FAiCharacterStats& ASimsAIController::GetAiCharacterStats()
{
	m_stats.Hungry = Blackboard->GetValueAsFloat("HungryFeeling");
	m_stats.Thirsty = Blackboard->GetValueAsFloat("ThirstyFeeling");
	m_stats.Cold = Blackboard->GetValueAsFloat("ColdFeeling");
	m_stats.Pee = Blackboard->GetValueAsFloat("PeeInside");
	m_stats.Stinks = Blackboard->GetValueAsFloat("StinksPercentage");
	m_stats.DidNothing = Blackboard->GetValueAsFloat("DidNothingPercentage");
	m_stats.Tired = Blackboard->GetValueAsFloat("TirednessFeeling");
	return m_stats;
}

void ASimsAIController::performMove(float deltaTime)
{
	m_currentMoveTime += deltaTime;

	if (m_currentMoveTime > OneMoveTime)
	{
		auto nextHex = m_currentPath.back();
		if (nextHex->GetTerrainType() == ETerrainType::TT_ClosedDoor)
		{
			nextHex->SetTerrainType(ETerrainType::TT_OpenDoor);
			m_possibleDestinations.Find(ETerrainType::TT_ClosedDoor)->Remove(nextHex);
			m_possibleDestinations.Find(ETerrainType::TT_OpenDoor)->Add(nextHex);
		}
		if(nextHex == m_destination && nextHex->GetTerrainType() == ETerrainType::TT_OpenDoor)
		{
			nextHex->SetTerrainType(ETerrainType::TT_ClosedDoor);
			m_possibleDestinations.Find(ETerrainType::TT_OpenDoor)->Remove(nextHex);
			m_possibleDestinations.Find(ETerrainType::TT_ClosedDoor)->Add(nextHex);
		}
		else
			moveActorTo(m_currentPath.back());

		m_currentPath.pop_back();
		m_currentMoveTime = 0;
	}
}

void ASimsAIController::moveActorTo(AHexagon* to)
{
	FVector new_location = to->GetActorLocation();
	new_location.Z = GetAiCharacter()->GetActorLocation().Z;

	FRotator new_rotation = UKismetMathLibrary::FindLookAtRotation(GetAiCharacter()->GetActorLocation(), new_location);
	GetAiCharacter()->SetActorLocationAndRotation(new_location, new_rotation);

	m_currentPosition = to;
}

bool ASimsAIController::initMove(ETerrainType type)
{
	m_destination = nullptr;
	m_currentPath.clear();

	//hopefully there is one of this type in the world
	if (!m_possibleDestinations.Find(type) || m_possibleDestinations.Find(type)->Num() == 0)
		return false;

	//calculate nearest path
	float bestCost = std::numeric_limits<float>::max();
	//auto possibleDestinations = m_possibleDestinations.FindRef(type);
	for (auto destination : m_possibleDestinations.FindRef(type))
	{
		std::unordered_map<AHexagon*, AHexagon*> came_from;
		std::unordered_map<AHexagon*, float> cost_so_far;
		Pathfinding::A_StarSearch(m_currentPosition, destination, came_from, cost_so_far, false);

		//is current calculated path the nearest possible?
		if (cost_so_far[destination] < bestCost)
		{
			bestCost = cost_so_far[destination];
			m_destination = destination;

			m_currentPath.clear();
			m_currentPath = Pathfinding::ReconstructPath(m_currentPosition, m_destination, came_from);
		}

	}

	//destination should be assigned!
	if (!m_destination || m_currentPath.size() == 0)
		return false;

	return true;
}