// Copyright Epic Games, Inc. All Rights Reserved.

#include "TFCATOCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TFCATOGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"
#include "Models/ObjectModel.h"
#include "Views/ObjectActor.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ATFCATOCharacter::ATFCATOCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void ATFCATOCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass.LoadSynchronous() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetClass is invalid, check %s"), *GetName());
		return;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	UUserWidget* HUD = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass.LoadSynchronous());
	if (HUD == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't create HUD with class - %s"), *HUDWidgetClass->GetName());
		return;
	}

	HUD->AddToViewport();
}

void ATFCATOCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATFCATOCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATFCATOCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ATFCATOCharacter::HandleInteract);

		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Triggered, this, &ATFCATOCharacter::ToggleUI);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATFCATOCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ATFCATOCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATFCATOCharacter::HandleInteract(const FInputActionValue& Value)
{
	FHitResult Hit;
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + FirstPersonCameraComponent->GetForwardVector() * 300.0f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	if (!bHit)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
		return;
	}

	AObjectActor* HitActor = Cast<AObjectActor>(Hit.GetActor());
	if (HitActor == nullptr)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 1.0f, 0, 1.0f);
		return;
	}

	const ATFCATOGameMode* GameMode = Cast<ATFCATOGameMode>(GetWorld()->GetAuthGameMode());
	UObjectModel* ObjectModel = GameMode->GetObjectModel();
	ObjectModel->ToggleActiveById(HitActor->GetObjectData().Id);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);
}

void ATFCATOCharacter::ToggleUI(const FInputActionValue& Value)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to toggle UI"));
		return;
	}

	if (PlayerController->ShouldShowMouseCursor())
	{
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	else
	{
		PlayerController->SetShowMouseCursor(true);

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		PlayerController->SetInputMode(InputMode);
	}
}
