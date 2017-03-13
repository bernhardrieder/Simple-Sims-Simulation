// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Hexagon.h"
#include <vector>
#include "AIController.h"
#include "SimsAIController.generated.h"

USTRUCT(BlueprintType)
struct FAiCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		float Hungry; 

	UPROPERTY(BlueprintReadOnly)
		float Thirsty;

	UPROPERTY(BlueprintReadOnly)
		float Cold;

	UPROPERTY(BlueprintReadOnly)
		float Pee;

	UPROPERTY(BlueprintReadOnly)
		float Stinks;

	UPROPERTY(BlueprintReadOnly)
		float DidNothing;

	UPROPERTY(BlueprintReadOnly)
		float Tired;
};

UCLASS()
class MYSIMS_API ASimsAIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Behaviour)
	class UBlackboardData* BlackboardData;

	UPROPERTY(EditAnywhere, Category = Behaviour)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = Behaviour)
		float OneMoveTime = 0.5f;

public:

	virtual void BeginPlay() override;
	class APathfinderCharacter* GetAiCharacter() const;
	bool MoveTo(ETerrainType type);
	void KeepMoving(float deltaTime);
	bool FinishedMoving() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	FAiCharacterStats& GetAiCharacterStats();

private:
	void performMove(float deltaTime);
	void moveActorTo(class AHexagon* to);
	bool initMove(ETerrainType type);

	class APathfinderCharacter* m_aiCharacter;
	TMap<ETerrainType, TArray<AHexagon*>> m_possibleDestinations;
	AHexagon* m_currentPosition;
	AHexagon* m_destination;
	float m_currentMoveTime;
	std::vector<class AHexagon*> m_currentPath;
	bool m_finishedMove = false;

	FAiCharacterStats m_stats;
};
