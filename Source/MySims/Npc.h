// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Hexagon.h"
#include "ThreadColliderComponent.h"
#include <map>
#include <vector>
#include "GameFramework/Actor.h"
#include "PathfinderMovementComponent.h"
#include "Npc.generated.h"

UCLASS()
class MYSIMS_API ANpc : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
		USceneComponent* Root;
	UPROPERTY(EditDefaultsOnly)
		USceneComponent* ThreatRadiusColliders;

	UPROPERTY(EditAnywhere)
		class UPathfinderMovementComponent* MovementController;

	UPROPERTY(EditAnywhere)
		int ThreatRadius = 5;

	UPROPERTY(EditAnywhere)
		UStaticMesh* HexagonMesh;

	UPROPERTY(EditAnywhere)
		AHexagon* PathStart;
	UPROPERTY(EditAnywhere)
		AHexagon* PathEnd;

public:
	ANpc();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	void OnThreatTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	int m_colliderCount = 0;
	float m_maxThreatCost;
	TArray<AHexagon*> m_currentThreatHexagons;

	void createThreatCollider();
	UThreadColliderComponent* createThreatColliderHexagon(FVector location);
	void resetCurrentThreatHexagons();
};
