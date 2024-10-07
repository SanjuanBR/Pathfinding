// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Player/PlayerInterface.h"
#include "PathfindingCharacter.generated.h"

class UPathfindingMovement;

UCLASS(Blueprintable)
class APathfindingCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	APathfindingCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void GoToLocation_Implementation(const FVector& Location) override;
	virtual void RotateCamera_Implementation(float DeltaYaw) override;

protected:
	virtual void BeginPlay() override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY()
	UPathfindingMovement* PathfindingMovement;

	UPROPERTY()
	FRotator InitialRotation;
	
	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, Category="Setup", meta=(AllowPrivateAccess=true))
	float RotationSpeed = 8.f;

	void UpdateCameraRotation(const float DeltaTime) const;
};

