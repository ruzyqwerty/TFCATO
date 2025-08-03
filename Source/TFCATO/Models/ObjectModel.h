// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ObjectModel.generated.h"

struct FObjectData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectUpdated, const FObjectData&, NewObjectState);

/**
 * Модель данных, загружающая и хранящая состояния объектов из JSON-файла.
 * Используется для инициализации и отслеживания интерактивных объектов в игре.
 */
UCLASS(Blueprintable)
class TFCATO_API UObjectModel : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Data")
	const TArray<FObjectData>& GetObjectDataList() const { return ObjectDataList; }

	UFUNCTION(BlueprintCallable)
	void ToggleActiveById(int32 Id);

	/** Загружает данные из JSON-файла */
	UFUNCTION(BlueprintCallable, Category = "Data")
	bool LoadFromJSON(const FString& FilePath);

	/** Сохраняет данные в JSON-файл */
	void SaveToJson(const FString& FilePath) const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnObjectUpdated OnObjectUpdated;

	/** Хранит данные, загруженные из JSON */
	UPROPERTY()
	TArray<FObjectData> ObjectDataList;
};
