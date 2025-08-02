// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectData.generated.h"

/**
 * Данные объекта, описывающие его положение, цвет и состояние активности в игре.
 */
USTRUCT(BlueprintType)
struct FObjectData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Id = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = FName("Name");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Color = FString("color");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive = false;
};
