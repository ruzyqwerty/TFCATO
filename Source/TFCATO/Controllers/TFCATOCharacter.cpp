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
#include "TFCATO/Models/ObjectModel.h"
#include "TFCATO/Views/ObjectActor.h"

DEFINE_LOG_CATEGORY(LogTFCATOCharacter);

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

	TSubclassOf<UUserWidget> LoadedHUDClass = HUDWidgetClass.LoadSynchronous();
	if (LoadedHUDClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: HUDWidgetClass is invalid, check %s"), __FUNCTION__, *GetName());
		return;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%hs :: Character is not for player, because controller is different - Controller is %s"), __FUNCTION__, *Controller->GetName());
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (EnhancedSubsystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: Can't get enhanced input subsystem from %s"), __FUNCTION__, *PlayerController->GetName());
		return;
	}

	EnhancedSubsystem->AddMappingContext(DefaultMappingContext, 0);

	// создаём и отображаем интерфейс
	UUserWidget* HUD = CreateWidget<UUserWidget>(GetWorld(), LoadedHUDClass);
	if (HUD == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: Can't create HUD with class - %s"), __FUNCTION__, *LoadedHUDClass->GetName());
		return;
	}

	HUD->AddToViewport();
}

void ATFCATOCharacter::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InInputComponent);
	if (EnhancedInputComponent == nullptr)
	{
		UE_LOG(LogTFCATOCharacter, Error, TEXT("%hs :: '%s' Failed to find an Enhanced Input Component!"), __FUNCTION__, *GetNameSafe(this));
		return;
	}

	EnhancedInputComponent->BindAction(JumpAction.LoadSynchronous(), ETriggerEvent::Started, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction.LoadSynchronous(), ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	EnhancedInputComponent->BindAction(MoveAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ATFCATOCharacter::Move);

	EnhancedInputComponent->BindAction(LookAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ATFCATOCharacter::Look);

	EnhancedInputComponent->BindAction(InteractAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ATFCATOCharacter::HandleInteract);

	EnhancedInputComponent->BindAction(TabAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ATFCATOCharacter::ToggleUI);
}

void ATFCATOCharacter::Move(const FInputActionValue& Value)
{
	if (Controller == nullptr)
	{
		return;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void ATFCATOCharacter::Look(const FInputActionValue& Value)
{
	if (Controller == nullptr)
	{
		return;
	}

	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ATFCATOCharacter::HandleInteract(const FInputActionValue& Value)
{
	FHitResult Hit;
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + FirstPersonCameraComponent->GetForwardVector() * 300.0f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// при нажатии клавиши Interact игрок попытается затрейсить вперёд, после чего проведёт проверку, что это наш ObjectActor
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
		return;
	}

	const AObjectActor* HitObjectActor = Cast<AObjectActor>(Hit.GetActor());
	if (HitObjectActor == nullptr)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 1.0f, 0, 1.0f);
		return;
	}

	// если все проверки успешно пройдены, то переключаем активность у объекта (включаем/выключаем отображение цвета)
	const ATFCATOGameMode* GameMode = Cast<ATFCATOGameMode>(GetWorld()->GetAuthGameMode());
	UObjectModel* ObjectModel = GameMode->GetObjectModel();
	ObjectModel->ToggleActiveById(HitObjectActor->GetObjectData().Id);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);
}

void ATFCATOCharacter::ToggleUI(const FInputActionValue& Value)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%hs :: Failed to toggle UI"), __FUNCTION__);
		return;
	}

	// переключаем отображение курсора, чтобы можно было взаимодействовать с интерфейсом (если включили)
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
