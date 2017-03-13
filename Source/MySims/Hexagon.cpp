// Fill out your copyright notice in the Description page of Project Settings.

#include "Pathfinder.h"
#include "Hexagon.h"
#include <limits>
#include <string>
#include "Components/TextRenderComponent.h"

#if WITH_EDITOR
void AHexagon::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr)	? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHexagon, TerrainType))
	{
		setTerrainSpecifics(TerrainType);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

AHexagon::AHexagon() : m_isBlinkingActivated(false)
{
	PrimaryActorTick.bCanEverTick = true;

	HexagonMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("HexagonMesh");
	HexagonMeshComponent->bGenerateOverlapEvents = true;
	HexagonMeshComponent->SetSimulatePhysics(false);

	RootComponent = HexagonMeshComponent;
	NeighbourColliderComponent = CreateDefaultSubobject<UStaticMeshComponent>("NeighbourCollider");
	NeighbourColliderComponent->bGenerateOverlapEvents = true;
	NeighbourColliderComponent->SetSimulatePhysics(false);
	NeighbourColliderComponent->SetWorldScale3D(FVector(1.2f, 1.2f, 1.f));
	NeighbourColliderComponent->SetStaticMesh(HexagonMeshComponent->GetStaticMesh());
	NeighbourColliderComponent->SetCollisionProfileName("OverlapAll");
	NeighbourColliderComponent->bVisible = false;
	NeighbourColliderComponent->bHiddenInGame = false;
	NeighbourColliderComponent->SetupAttachment(RootComponent);

	ThreadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ThreadMesh");
	ThreadMeshComponent->bGenerateOverlapEvents = false;
	ThreadMeshComponent->SetSimulatePhysics(false);
	ThreadMeshComponent->SetupAttachment(RootComponent);
	ThreadMeshComponent->SetWorldLocation(FVector(0, 0, 100));
	ThreadMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.01f));
	ThreadMeshComponent->SetCollisionProfileName("NoCollision");
	ThreadMeshComponent->SetHiddenInGame(true);
	ThreadMeshComponent->SetVisibility(false);


	Text = CreateDefaultSubobject<UTextRenderComponent>("TextRender");
	Text->SetWorldRotation(FRotator(90, 0, 180));
	Text->SetupAttachment(RootComponent);
	Text->SetVerticalAlignment(EVRTA_TextCenter);
	Text->SetHorizontalAlignment(EHTA_Center);
	Text->SetTextRenderColor(FColor::White);
	Text->Text = FText::FromString("");

	TerrainType = ETerrainType::TT_Street;
	setTerrainSpecifics(TerrainType);
	m_threatCost = 0;
}

void AHexagon::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> overlapping;
	NeighbourColliderComponent->GetOverlappingActors(overlapping, TSubclassOf<AHexagon>());
	for(auto actor : overlapping)
	{
		auto hex = Cast<AHexagon>(actor);
		if (hex && actor->GetName() != this->GetName() && hex->IsWalkable())
			Neighbours.Add(Cast<AHexagon>(actor));
	}

	m_dynamicMaterial = HexagonMeshComponent->CreateDynamicMaterialInstance(0, BaseMaterial);
	m_threatDynamicMaterial = ThreadMeshComponent->CreateDynamicMaterialInstance(0, BaseMaterial);
	setTerrainSpecifics(TerrainType);
	ThreadMeshComponent->SetStaticMesh(HexagonMeshComponent->GetStaticMesh());
}

void AHexagon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (m_isBlinkingActivated)
		blink(DeltaTime);
}

ETerrainType AHexagon::GetTerrainType() const
{
	return TerrainType;
}

void AHexagon::SetTerrainType(ETerrainType type)
{
	TerrainType = type;
	setTerrainSpecifics(type);
}

float AHexagon::GetCost(bool withThreat) const
{
	if(withThreat)
		return static_cast<float>(TerrainType) + m_threatCost;
	return static_cast<float>(TerrainType);
}

float AHexagon::GetTerrainCost() const
{
	return static_cast<float>(TerrainType);
}

float AHexagon::GetThreatCost() const
{
	return m_threatCost;
}

void AHexagon::SetPathColor(bool val, FColor color)
{
	if (val)
		SetColor(to_color(TerrainType), 3);
	else
		SetTerrainColor();
}

void AHexagon::SetColor(FColor color, float emission)
{
	m_dynamicMaterial->SetVectorParameterValue("BaseColor", FLinearColor(color));
	SetEmission(emission);
	if (color == FColor::White)
		Text->SetTextRenderColor(FColor::Black);
	else
		Text->SetTextRenderColor(FColor::White);
}

void AHexagon::SetTerrainColor()
{
	SetColor(to_color(TerrainType));
}

void AHexagon::SetDestinationColor(FColor color)
{
	SetColor(color);
}

bool AHexagon::IsWalkable() const
{
	return static_cast<int>(TerrainType) != 0;
}

void AHexagon::SetThreat(float cost, FLinearColor color)
{
	m_isThreat = cost !=0;
	m_threatCost = cost;
	ThreadMeshComponent->SetHiddenInGame(!m_isThreat);
	ThreadMeshComponent->SetVisibility(m_isThreat);
	SetThreatColor(color);
}

void AHexagon::AddThreatCost(float cost)
{
	m_threatCost += cost;
	m_isThreat = m_threatCost != 0;
	ThreadMeshComponent->SetHiddenInGame(!m_isThreat);
	ThreadMeshComponent->SetVisibility(m_isThreat);
}

void AHexagon::SetThreatColor(FLinearColor color)
{
	m_threatDynamicMaterial->SetVectorParameterValue("BaseColor", FLinearColor(color));
}

void AHexagon::ActivateBlinking(bool val, bool resetEmission)
{
	m_isBlinkingActivated = val;
	if(resetEmission)
		SetEmission(0);
}

void AHexagon::SetEmission(float emission)
{
	m_dynamicMaterial->SetScalarParameterValue("Emission", emission);
	m_currentDestinationEmission = emission;
}

bool AHexagon::IsThreat() const
{
	return m_isThreat;
}

void AHexagon::setTerrainSpecifics(ETerrainType type)
{
	if (!BaseMaterial) return;
	m_dynamicMaterial = HexagonMeshComponent->CreateDynamicMaterialInstance(0, BaseMaterial);
	SetTerrainColor();

	Text->SetText(FText::FromString(to_string(type).c_str()));
	FVector origin = this->GetActorLocation();
	origin.Z = 0;
	if (TerrainType == ETerrainType::TT_Mountain || TerrainType == ETerrainType::TT_ClosedDoor)
	{
		HexagonMeshComponent->SetWorldScale3D(FVector(1.f, 1.f, 2.f));

		this->SetActorLocation(origin + FVector(0.f, 0.f, HexagonMeshComponent->GetStaticMesh()->GetBounds().BoxExtent.Z));
		Text->SetWorldLocation(origin + FVector(0.f, 0.f, 10.f + HexagonMeshComponent->GetStaticMesh()->GetBounds().BoxExtent.Z*3));
	}
	else
	{
		HexagonMeshComponent->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
		this->SetActorLocation(origin);
		Text->SetWorldLocation(origin + FVector(0.f, 0.f, 10.f + HexagonMeshComponent->GetStaticMesh()->GetBounds().BoxExtent.Z));
	}
}

void AHexagon::blink(float deltaTime)
{
	SetEmission(m_currentDestinationEmission + m_emissionDelta);
	if (m_currentDestinationEmission < 0 || m_currentDestinationEmission > 10)
		m_emissionDelta *= -1;
}
