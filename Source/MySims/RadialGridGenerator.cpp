// Fill out your copyright notice in the Description page of Project Settings.

#include "Pathfinder.h"
#include "RadialGridGenerator.h"
#include "Hexagon.h"


#if WITH_EDITOR
void ARadialGridGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (HexagonBP && PropertyName == GET_MEMBER_NAME_CHECKED(ARadialGridGenerator, CreateGrid) && CreateGrid)
	{
		createGrid();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

// Sets default values
ARadialGridGenerator::ARadialGridGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARadialGridGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARadialGridGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ARadialGridGenerator::createGrid()
{
	FBoxSphereBounds hexBounds;
	TArray<AActor*> middleColumnActors;

	//middle column
	for (int row = -Radius; row <= Radius; ++row)
	{
		auto hexActor = GetWorld()->SpawnActor(HexagonBP);
#if WITH_EDITOR
		hexActor->SetFolderPath("/RadialHexGrid");
#endif
		middleColumnActors.Add(hexActor);

		if (row == -Radius)
		{
			auto hex = Cast<AHexagon>(hexActor);
			if (hex)
				hexBounds = hex->GetMeshComponent()->GetStaticMesh()->GetBounds();
		}

		float yCoord = hexBounds.BoxExtent.Y * 2 * row;
		hexActor->SetActorLocation(FVector(0.f, yCoord, 0.f));
	}

	for (int column = -Radius; column <= Radius; ++column)
	{
		if (column != 0)
		{
			for (int i = 0; i < middleColumnActors.Num() - FMath::Abs(column); ++i)
			{
				FVector coord = middleColumnActors[i]->GetActorLocation();
				coord.X += (1.5f * hexBounds.BoxExtent.X * column);
				coord.Y += hexBounds.BoxExtent.Y *FMath::Abs(column);
				auto hexActor = GetWorld()->SpawnActor(HexagonBP);
#if WITH_EDITOR
				hexActor->SetFolderPath("/RadialHexGrid");
#endif
				hexActor->SetActorLocation(coord);
			}
		}
	}
}
