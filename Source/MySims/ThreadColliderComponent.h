// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "ThreadColliderComponent.generated.h"

/**
 * 
 */
UCLASS()
class MYSIMS_API UThreadColliderComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	float ThreatCost;
};
