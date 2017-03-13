// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include "GameFramework/Actor.h"
#include "Hexagon.generated.h"

UENUM(BlueprintType)
enum class ETerrainType : uint8
{
	TT_Mountain = 0		UMETA(DisplayName = "Mountain"),
	TT_Grass = 20		UMETA(DisplayName = "Grass"),
	TT_Sand = 30		UMETA(DisplayName = "Sand"),
	TT_Mud = 40			UMETA(DisplayName = "Mud"),
	TT_Water = 50		UMETA(DisplayName = "Water"),
	TT_Street = 59		UMETA(DisplayName = "Street"),

	//Sims objects
	TT_OpenDoor	= 60	UMETA(DisplayName = "OpenDoor"),
	TT_ClosedDoor		UMETA(DisplayName = "ClosedDoor"),
	TT_Toilet			UMETA(DisplayName = "Toilet"),
	TT_Bath				UMETA(DisplayName = "Bath"),
	TT_Bed				UMETA(DisplayName = "Bed"),
	TT_Weights			UMETA(DisplayName = "Weights"),
	TT_Cooker = 150		UMETA(DisplayName = "Cooker"),
	TT_PC				UMETA(DisplayName = "PC"),
	TT_Frigde			UMETA(DisplayName = "Fridge"),
	TT_WaterTap			UMETA(DisplayName = "WaterTap"),
	TT_Fireplace		UMETA(DisplayName = "Fireplace"),
	TT_Table			UMETA(DisplayName = "Table")
};

ETerrainType const MySimsObjects[] = {ETerrainType::TT_OpenDoor, ETerrainType::TT_ClosedDoor,ETerrainType::TT_Toilet, ETerrainType::TT_Bath, ETerrainType::TT_PC, ETerrainType::TT_Frigde, ETerrainType::TT_Bed, ETerrainType::TT_Cooker, ETerrainType::TT_WaterTap, ETerrainType::TT_Weights, ETerrainType::TT_Fireplace, ETerrainType::TT_Table };

static FColor to_color(ETerrainType type)
{
	switch (type)
	{
	case ETerrainType::TT_Mountain: return FColor(159, 182, 205); //slategray
	case ETerrainType::TT_Grass: return FColor(34, 139, 34); //forrestgreen
	case ETerrainType::TT_Sand: return FColor(255, 215, 0); //gold
	case ETerrainType::TT_Water: return FColor(28, 134, 238); //dodgerblue
	case ETerrainType::TT_Street: return FColor(105, 105, 105); //grey
	case ETerrainType::TT_Mud: return FColor(139, 69, 19); //brown
	case ETerrainType::TT_OpenDoor: return FColor(139, 69, 19); //brown
	case ETerrainType::TT_ClosedDoor: return FColor(139, 69, 19); //brown
	case ETerrainType::TT_Toilet: return FColor::Cyan;
	case ETerrainType::TT_Bath: return FColor::Turquoise;
	case ETerrainType::TT_PC: return FColor::Magenta;
	case ETerrainType::TT_Frigde: return FColor::Yellow;
	case ETerrainType::TT_Bed: return FColor::White;
	case ETerrainType::TT_Cooker: return FColor::Orange;
	case ETerrainType::TT_WaterTap: return FColor::Blue;
	case ETerrainType::TT_Weights: return FColor::Black;
	case ETerrainType::TT_Fireplace: return FColor::Red;
	case ETerrainType::TT_Table: return FColor(165, 42, 42); //brown
		default:
		return FColor::Transparent;
	}
}

static std::string to_string(ETerrainType type)
{
	switch (type)
	{
	case ETerrainType::TT_Mountain: return "Wall";
	case ETerrainType::TT_Grass: return "Grass";
	case ETerrainType::TT_Sand: return "Sand";
	case ETerrainType::TT_Water: return "Water";
	case ETerrainType::TT_Street: return "Floor";
	case ETerrainType::TT_Mud: return "Mud";
	case ETerrainType::TT_OpenDoor: return "Open Door";
	case ETerrainType::TT_ClosedDoor: return "Closed Door";
	case ETerrainType::TT_Toilet: return "Toilet";
	case ETerrainType::TT_Bath: return "Bath";
	case ETerrainType::TT_PC: return "PC";
	case ETerrainType::TT_Frigde: return "Fridge";
	case ETerrainType::TT_Bed: return "Bed";
	case ETerrainType::TT_Cooker: return "Cooker";
	case ETerrainType::TT_WaterTap: return "Water Tap";
	case ETerrainType::TT_Weights: return "Weights";
	case ETerrainType::TT_Fireplace: return "Fireplace";
	case ETerrainType::TT_Table: return "Table";
	default:
		return "Unknown";
	}
}

UCLASS()
class MYSIMS_API AHexagon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Hexagon)
	UStaticMeshComponent* HexagonMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = Hexagon)
		UStaticMeshComponent* ThreadMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = Hexagon)
	UStaticMeshComponent* NeighbourColliderComponent;

	UPROPERTY(EditAnywhere, Category = Hexagon)
		ETerrainType TerrainType;

	UPROPERTY(EditAnywhere, Category = Hexagon)
		UMaterial* BaseMaterial;

	UPROPERTY(EditAnywhere, Category = Hexagon)
	class UTextRenderComponent* Text;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	AHexagon();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	UStaticMeshComponent* GetMeshComponent() const { return HexagonMeshComponent; };

	ETerrainType GetTerrainType() const;
	void SetTerrainType(ETerrainType type);
	float GetCost(bool withThreat = true) const;
	float GetTerrainCost() const;
	float GetThreatCost() const;
	void SetPathColor(bool val, FColor color = FColor::Orange);
	void SetColor(FColor color, float emission = 0);
	void SetTerrainColor();
	void SetDestinationColor(FColor color = FColor::Red);
	bool IsWalkable() const;
	void SetThreat(float cost, FLinearColor color);
	void AddThreatCost(float cost);
	void SetThreatColor(FLinearColor color);
	void ActivateBlinking(bool val, bool resetEmission = true);
	void SetEmission(float emission);
	bool IsThreat() const;

	TArray<AHexagon*> Neighbours;

private:
	void setTerrainSpecifics(ETerrainType type);
	void blink(float deltaTime);

	UMaterialInstanceDynamic* m_dynamicMaterial;
	UMaterialInstanceDynamic* m_threatDynamicMaterial;
	float m_threatCost = 0;
	bool m_isBlinkingActivated;
	float m_currentDestinationEmission = 0;
	float m_emissionDelta = 0.3f;
	bool m_isThreat = false;

};
