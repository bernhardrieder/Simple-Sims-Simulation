// Fill out your copyright notice in the Description page of Project Settings.

#include "Pathfinder.h"
#include "Npc.h"
#include "Hexagon.h"
#include <string>

ANpc::ANpc()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	MovementController = CreateDefaultSubobject<UPathfinderMovementComponent>("MovementController");
	MovementController->ActorToMove = this;
	MovementController->bVisualizePath = false;
	MovementController->bConsiderThreat = false;
	MovementController->bLerpMovement = false;

	ThreatRadiusColliders = CreateDefaultSubobject<USceneComponent>("ThreatRadiusColliders");
	ThreatRadiusColliders->SetupAttachment(RootComponent);
	MovementController->OnBeforeReachDestination = [&]() {resetCurrentThreatHexagons(); };
}

void ANpc::BeginPlay()
{
	Super::BeginPlay();
	createThreatCollider();
	MovementController->MoveInstantly(PathStart);
	MovementController->Move(PathStart, PathEnd, true);
}

void ANpc::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ANpc::OnThreatTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto hex = Cast<AHexagon>(OtherActor);
	if (hex && hex->IsWalkable())
	{
		auto comp = Cast<UThreadColliderComponent>(OverlappedComp);
		hex->SetThreat(comp->ThreatCost, FColor());
		static FColor red = FColor::Red;
		static FColor yellow = FColor::Yellow;
		float alpha = comp->ThreatCost / m_maxThreatCost;
		FColor color = FColor(FMath::Lerp(red.R, yellow.R, alpha), FMath::Lerp(red.G, yellow.G, alpha), FMath::Lerp(red.B, yellow.B, alpha));
		hex->SetThreatColor(color);

		m_currentThreatHexagons.Add(hex);
	}
}

void ANpc::createThreatCollider()
{
	FBoxSphereBounds hexBounds = HexagonMesh->GetBounds();
	TArray<UStaticMeshComponent*> middleColumnActors;

	//middle column
	for (int row = -ThreatRadius; row <= ThreatRadius; ++row)
	{
		float yCoord = hexBounds.BoxExtent.Y * 2 * row;
		UThreadColliderComponent* collider = createThreatColliderHexagon(FVector(0.f, yCoord, 0.f));
		middleColumnActors.Add(collider);
	}

	for (int column = -ThreatRadius; column <= ThreatRadius; ++column)
	{
		if (column != 0)
		{
			for (int i = 0; i < middleColumnActors.Num() - FMath::Abs(column); ++i)
			{
				FVector coord = middleColumnActors[i]->GetComponentLocation();
				coord.X += (1.5f * hexBounds.BoxExtent.X * column);
				coord.Y += hexBounds.BoxExtent.Y *FMath::Abs(column);
				createThreatColliderHexagon(coord);
			}
		}
	}
}

UThreadColliderComponent* ANpc::createThreatColliderHexagon(FVector location)
{
	UThreadColliderComponent* collider = NewObject<UThreadColliderComponent>(this, UThreadColliderComponent::StaticClass(), std::to_string(++m_colliderCount).c_str());
	collider->RegisterComponent();
	collider->SetSimulatePhysics(false);
	collider->bGenerateOverlapEvents = true;
	collider->SetCollisionProfileName("OverlapAll");
	collider->SetStaticMesh(HexagonMesh);
	collider->SetWorldLocation(location);
	collider->SetWorldScale3D(FVector(1.0f, 1.0f, 2.0f));
	collider->AttachToComponent(ThreatRadiusColliders, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
	collider->bVisible = false;
	collider->OnComponentBeginOverlap.AddDynamic(this, &ANpc::OnThreatTriggerOverlapBegin);

	float dist = FVector::Dist(ThreatRadiusColliders->GetComponentLocation(), collider->GetComponentLocation());
	m_maxThreatCost = m_maxThreatCost > dist ? m_maxThreatCost : dist;
	collider->ThreatCost = FMath::Abs(dist);

	return collider;
}

void ANpc::resetCurrentThreatHexagons()
{
	for(auto a: m_currentThreatHexagons)
	{
		a->SetThreat(0, FColor());
	}
	m_currentThreatHexagons.Reset();
}
