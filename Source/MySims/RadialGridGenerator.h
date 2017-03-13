// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RadialGridGenerator.generated.h"

UCLASS()
class MYSIMS_API ARadialGridGenerator : public AActor
{
	GENERATED_BODY()
	
#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditAnywhere)
		int Radius;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> HexagonBP;
	UPROPERTY(EditAnywhere)
		bool CreateGrid;

public:	
	// Sets default values for this actor's properties
	ARadialGridGenerator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	void createGrid();
	
};
