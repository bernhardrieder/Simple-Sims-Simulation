// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"
#include "PathfinderCharacter.generated.h"

UCLASS(Blueprintable)
class APathfinderCharacter : public ACharacter
{
	GENERATED_BODY()
	

public:
	APathfinderCharacter();

	// Called every frame.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	//void SetStatsText(FText text);
	UFUNCTION(BlueprintCallable, Category = "AI")
	class ASimsAIController* GetAiController() const;

	void SetAiController(class ASimsAIController* controller);
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float MaxZoomIn = 200.f;
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float MaxZoomOut = 1000.f;
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float ZoomDelta = 100.f;

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
protected:
	void CameraZoomIn();
	void CameraZoomOut();

	float m_currentCameraZoom;
	class AHexagon* m_possibleDestination;
	class ASimsAIController* m_aiController;
};

