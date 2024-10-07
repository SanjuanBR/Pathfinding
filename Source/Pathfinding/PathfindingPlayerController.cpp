// Copyright Epic Games, Inc. All Rights Reserved.

#include "PathfindingPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "PathfindingCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "PathfindingGrid/PathfindingBlueprintLibrary.h"
#include "Player/PlayerInterface.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

APathfindingPlayerController::APathfindingPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void APathfindingPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void APathfindingPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &APathfindingPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &APathfindingPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &APathfindingPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &APathfindingPlayerController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this, &APathfindingPlayerController::RotateCamera);
		EnhancedInputComponent->BindAction(EnableRotationAction, ETriggerEvent::Started, this, &APathfindingPlayerController::EnableRotation);
		EnhancedInputComponent->BindAction(EnableRotationAction, ETriggerEvent::Completed, this, &APathfindingPlayerController::EnableRotation);
		EnhancedInputComponent->BindAction(EnableRotationAction, ETriggerEvent::Canceled, this, &APathfindingPlayerController::EnableRotation);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &APathfindingPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &APathfindingPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &APathfindingPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &APathfindingPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APathfindingPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void APathfindingPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void APathfindingPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		APawn* ControlledPawn = GetPawn();
		if(ControlledPawn && ControlledPawn->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_GoToLocation(ControlledPawn, CachedDestination);
		}
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void APathfindingPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void APathfindingPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void APathfindingPlayerController::RotateCamera(const FInputActionValue& Value)
{
	if(!bIsRotationEnabled){return;}
		
	const float DeltaYaw = Value.Get<float>();

	APawn* ControlledPawn = GetPawn();
	if(ControlledPawn && ControlledPawn->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_RotateCamera(ControlledPawn, DeltaYaw);
	}
}

void APathfindingPlayerController::EnableRotation(const FInputActionValue& Value)
{
	bIsRotationEnabled = Value.Get<bool>();
}
