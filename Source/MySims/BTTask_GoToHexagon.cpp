// Fill out your copyright notice in the Description page of Project Settings.

#include "Pathfinder.h"
#include "BTTask_GoToHexagon.h"
#include "Hexagon.h"
#include "EngineUtils.h"
#include "Pathfinding.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/Tasks/BTTask_WaitBlackboardTime.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "Kismet/KismetMathLibrary.h"
#include "SimsAIController.h"

EBTNodeResult::Type UBTTask_GoToHexagon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	m_controller = Cast<ASimsAIController>(OwnerComp.GetOwner());
	if (m_controller->MoveTo(ObjectType))
	{
		bNotifyTick = true;
		return EBTNodeResult::InProgress;
	}
	bNotifyTick = false;
	return EBTNodeResult::Failed;
}

void UBTTask_GoToHexagon::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//move with each tick
	if (!m_controller->FinishedMoving())
	{
		m_controller->KeepMoving(DeltaSeconds);
	}
	else //destination reached
	{
		// continue execution from this node
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}