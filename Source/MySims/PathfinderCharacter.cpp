// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Pathfinder.h"
#include "PathfinderCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Hexagon.h"
#include "PathfinderPlayerController.h"
#include "Components/TextRenderComponent.h"
#include "SimsAIController.h"

APathfinderCharacter::APathfinderCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = MaxZoomIn;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	m_currentCameraZoom = CameraBoom->TargetArmLength;

										  // Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	
}

void APathfinderCharacter::BeginPlay()
{
	Super::BeginPlay();
	CameraBoom->TargetArmLength = MaxZoomIn;
	m_currentCameraZoom = CameraBoom->TargetArmLength;
}

void APathfinderCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	//if (CursorToWorld != nullptr)
	//{
	//	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	//	{
	//		FHitResult TraceHitResult;
	//		PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
	//		FVector CursorFV = TraceHitResult.ImpactNormal;
	//		FRotator CursorR = CursorFV.Rotation();
	//		CursorToWorld->SetWorldLocation(TraceHitResult.Location);
	//		CursorToWorld->SetWorldRotation(CursorR);

	//		m_possibleDestination = Cast<AHexagon>(TraceHitResult.Actor.Get());
	//	}
	//}
}

void APathfinderCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &APathfinderCharacter::CameraZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &APathfinderCharacter::CameraZoomOut);
	//InputComponent->BindAction("ToggleAiStats", IE_Pressed, this, &APathfinderCharacter::ToggleShowAiStats);
	//InputComponent->BindAction("SetDestination", IE_Pressed, this, &APathfinderCharacter::OnSetDestinationPressed);
}

ASimsAIController* APathfinderCharacter::GetAiController() const
{
	return m_aiController;
}

void APathfinderCharacter::SetAiController(ASimsAIController* controller)
{
	m_aiController = controller;
}

void APathfinderCharacter::OnSetDestinationPressed()
{
	//auto controller = Cast<APathfinderPlayerController>(GetController());
	//controller->SetNewDestination(m_possibleDestination);
}

void APathfinderCharacter::CameraZoomIn()
{
	m_currentCameraZoom = m_currentCameraZoom - ZoomDelta;

	if (m_currentCameraZoom <= MaxZoomIn)
		m_currentCameraZoom = MaxZoomIn;
	
	CameraBoom->TargetArmLength = m_currentCameraZoom;
}

void APathfinderCharacter::CameraZoomOut()
{
	m_currentCameraZoom = m_currentCameraZoom + ZoomDelta;

	if (m_currentCameraZoom >= MaxZoomOut)
		m_currentCameraZoom = MaxZoomOut;

	CameraBoom->TargetArmLength = m_currentCameraZoom;
}