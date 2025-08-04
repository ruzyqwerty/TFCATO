// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TFCATO/Models/ObjectData.h"
#include "ObjectActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogObjectActor, Log, All);

UCLASS()
class TFCATO_API AObjectActor : public AActor
{
	GENERATED_BODY()

public:
	AObjectActor();

	void InitializeFromData(const FObjectData& InData);

	const FObjectData& GetObjectData() const { return ObjectData; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

private:
	UFUNCTION()
	void OnModelUpdated(const FObjectData& NewData);

	void UpdateColor() const;

private:

	/** Данные, переданные при инициализации */
	FObjectData ObjectData;

	/** Материал, который будем динамически менять по цвету */
	TWeakObjectPtr<UMaterialInstanceDynamic> DynamicMaterial = nullptr;
};
