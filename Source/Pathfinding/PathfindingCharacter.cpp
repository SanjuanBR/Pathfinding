// Copyright Epic Games, Inc. All Rights Reserved.

#include "PathfindingCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Debug/DebugHelper.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "PathfindingGrid/PathfindingBlueprintLibrary.h"
#include "Player/PathfindingMovement.h"

APathfindingCharacter::APathfindingCharacter()
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
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PathfindingMovement = CreateDefaultSubobject<UPathfindingMovement>(TEXT("PathfindingMovement"));

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void APathfindingCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitialRotation = CameraBoom->GetRelativeRotation();
	TargetRotation = InitialRotation;
}

void APathfindingCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	UpdateCameraRotation(DeltaSeconds);
}

void APathfindingCharacter::GoToLocation_Implementation(const FVector& Location)
{
	IPlayerInterface::GoToLocation_Implementation(Location);

	const TArray<FVector> Path = UPathfindingBlueprintLibrary::GetPath(GetActorLocation(), Location, this);
	PathfindingMovement->MoveThoughPath(Path);
}

void APathfindingCharacter::RotateCamera_Implementation(float DeltaYaw)
{
	IPlayerInterface::RotateCamera_Implementation(DeltaYaw);

	const FQuat ArmDestinationQuat = TargetRotation.Quaternion();
	const FQuat DeltaRotation = FRotator(0,DeltaYaw * RotationSpeed,0).Quaternion();
	TargetRotation = (ArmDestinationQuat * DeltaRotation).Rotator();
	TargetRotation = FRotator(TargetRotation.Pitch,TargetRotation.Yaw,0.f);

	TargetRotation.Yaw = FMath::ClampAngle(TargetRotation.Yaw, InitialRotation.Yaw - 90.f, InitialRotation.Yaw + 90.f);
}

void APathfindingCharacter::UpdateCameraRotation(const float DeltaTime) const
{
	const FRotator NewRotation = FMath::RInterpTo(CameraBoom->GetRelativeRotation(), TargetRotation, DeltaTime, 5.0f);
	CameraBoom->SetRelativeRotation(NewRotation);
}
