// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectModel.h"
#include "EngineUtils.h"
#include "ObjectData.h"
#include "TFCATO/Views/ObjectActor.h"

DEFINE_LOG_CATEGORY(LogObjectModel);

void UObjectModel::ToggleActiveById(const int32 Id)
{
	for (FObjectData& Data : ObjectDataList)
	{
		if (Data.Id != Id)
		{
			continue;
		}

		Data.bIsActive = !Data.bIsActive;
		OnObjectUpdated.Broadcast(Data);
		break;
	}
}

bool UObjectModel::LoadFromJSON(const FString& FilePath)
{
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogObjectModel, Error, TEXT("%hs :: Failed to load JSON file: %s"), __FUNCTION__, *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		UE_LOG(LogObjectModel, Error, TEXT("%hs :: Can't deserialize file %s "), __FUNCTION__, *FilePath);
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* ObjectsArray;
	if (!JsonObject->TryGetArrayField(TEXT("objects"), ObjectsArray))
	{
		UE_LOG(LogObjectModel, Error, TEXT("%hs :: Invalid JSON format: Can't find 'objects' field in %s"), __FUNCTION__, *FilePath);
		return false;
	}

	ObjectDataList.Empty();
	for (const TSharedPtr<FJsonValue>& Value : *ObjectsArray)
	{
		const TSharedPtr<FJsonObject>* ObjectFields;
		if (!Value->TryGetObject(ObjectFields)) continue;

		FObjectData NewData;

		// получаем поле id и записываем в NewData
		(*ObjectFields)->TryGetNumberField("id", NewData.Id);

		// получаем поле name, так как оно хранится в виде строк, то конвертируем в FName
		FString NameStr;
		if ((*ObjectFields)->TryGetStringField("name", NameStr))
		{
			NewData.Name = FName(NameStr);
		}

		// получаем поля color и isActive
		(*ObjectFields)->TryGetStringField("color", NewData.Color);
		(*ObjectFields)->TryGetBoolField("isActive", NewData.bIsActive);

		// получаем поле position, оно хранит список из 3-ёх чисел, конвертируем их в один FVector
		const TArray<TSharedPtr<FJsonValue>>* PositionArray;
		if ((*ObjectFields)->TryGetArrayField("position", PositionArray) && PositionArray->Num() == 3)
		{
			const float X = static_cast<float>((*PositionArray)[0]->AsNumber());
			const float Y = static_cast<float>((*PositionArray)[1]->AsNumber());
			const float Z = static_cast<float>((*PositionArray)[2]->AsNumber());
			NewData.Position = FVector(X, Y, Z);
		}

		ObjectDataList.Add(NewData);
	}

	return true;
}

void UObjectModel::SaveToJson(const FString& FilePath) const
{
	TArray<TSharedPtr<FJsonValue>> ObjectArray;
	for (const FObjectData& Data : ObjectDataList)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		JsonObject->SetNumberField("id", Data.Id);
		JsonObject->SetStringField("name", Data.Name.ToString());
		JsonObject->SetStringField("color", Data.Color);
		JsonObject->SetBoolField("isActive", Data.bIsActive);

		TArray<TSharedPtr<FJsonValue>> PositionArray;
		PositionArray.Add(MakeShared<FJsonValueNumber>(Data.Position.X));
		PositionArray.Add(MakeShared<FJsonValueNumber>(Data.Position.Y));
		PositionArray.Add(MakeShared<FJsonValueNumber>(Data.Position.Z));
		JsonObject->SetArrayField("position", PositionArray);

		ObjectArray.Add(MakeShared<FJsonValueObject>(JsonObject));
	}

	const TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetArrayField("objects", ObjectArray);

	FString OutputString;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

	if (FFileHelper::SaveStringToFile(OutputString, *FilePath))
	{
		UE_LOG(LogObjectModel, Log, TEXT("%hs :: Saved JSON to: %s"), __FUNCTION__, *FilePath);
	}
	else
	{
		UE_LOG(LogObjectModel, Error, TEXT("%hs :: Failed to save JSON to: %s"), __FUNCTION__, *FilePath);
	}
}
