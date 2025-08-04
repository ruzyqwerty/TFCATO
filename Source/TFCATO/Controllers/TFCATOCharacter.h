// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TFCATOCharacter.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInputComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTFCATOCharacter, Log, All);

UCLASS(config=Game)
class ATFCATOCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ATFCATOCharacter();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(const bool bNewHasRifle) { bHasRifle = bNewHasRifle; }

	UFUNCTION(BlueprintPure, Category = Weapon)
	bool GetHasRifle() const { return bHasRifle; }

	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	USkeletalMeshComponent* Mesh1P = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Input", meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Input", meta=(AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Input", meta=(AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Input", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> LookAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Input", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> InteractAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Input")
	TSoftObjectPtr<UInputAction> TabAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|UI")
	TSoftClassPtr<UUserWidget> HUDWidgetClass = nullptr;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Begin ACharacter override
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
	// End of ACharacter override

private:
	void HandleInteract(const FInputActionValue& Value);

	void ToggleUI(const FInputActionValue& Value);

private:
	bool bHasRifle = false;
};

