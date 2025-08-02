// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ObjectModel.generated.h"

struct FObjectData;

/**
 * Модель данных, загружающая и хранящая состояния объектов из JSON-файла.
 * Используется для инициализации и отслеживания интерактивных объектов в игре.
 */
UCLASS()
class TFCATO_API UObjectModel : public UObject
{
	GENERATED_BODY()

public:
	/** Хранит данные, загруженные из JSON */
	UPROPERTY()
	TArray<FObjectData> ObjectDataList;

	/** Загружает данные из JSON-файла */
	UFUNCTION(BlueprintCallable, Category = "Data")
	bool LoadFromJSON(const FString& FilePath);

	/** Сохраняет данные в JSON-файл */
	void SaveToJson(const FString& FilePath) const;
};
