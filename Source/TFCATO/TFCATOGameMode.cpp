// Copyright Epic Games, Inc. All Rights Reserved.

#include "TFCATOGameMode.h"
#include "TFCATOCharacter.h"
#include "Models/ObjectData.h"
#include "Models/ObjectModel.h"
#include "Views/ObjectActor.h"

ATFCATOGameMode::ATFCATOGameMode() : Super()
{
	DefaultPawnClass = ATFCATOCharacter::StaticClass();
}

void ATFCATOGameMode::BeginPlay()
{
	Super::BeginPlay();

	ObjectModel = NewObject<UObjectModel>(this);
	if (ObjectModel == nullptr)
	{
		return;
	}

	const FString SavedPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Data/ObjectStates.json"));
	const FString DefaultPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data/InitialData.json"));

	if (!FPaths::FileExists(SavedPath))
	{
		if (!FPlatformFileManager::Get().GetPlatformFile().CopyFile(*SavedPath, *DefaultPath))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to copy default JSON to Saved folder"));
			return;
		}
	}

	if (!ObjectModel->LoadFromJSON(SavedPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load JSON data"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Loaded %d objects from JSON"), ObjectModel->ObjectDataList.Num());

	for (const FObjectData& Data : ObjectModel->ObjectDataList)
	{
		UE_LOG(LogTemp, Log, TEXT("Object: %s | Color: %s | Active: %s"), *Data.Name.ToString(), *Data.Color, Data.bIsActive ? TEXT("true") : TEXT("false"));
	}

	for (const FObjectData& Data : ObjectModel->ObjectDataList)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Data.Position);

		AObjectActor* SpawnedActor = GetWorld()->SpawnActor<AObjectActor>(AObjectActor::StaticClass(), Data.Position, FRotator::ZeroRotator);
		if (SpawnedActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor: %s"), *Data.Name.ToString());
			continue;
		}

		SpawnedActor->InitializeFromData(Data);
	}
}

void ATFCATOGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Data/ObjectStates.json"));
	ObjectModel->SaveToJson(FilePath);
	
	Super::EndPlay(EndPlayReason);
}

UStaticMesh* ATFCATOGameMode::GetMeshByName(const FString& Name) const
{
	for (const TPair<FString, TSoftObjectPtr<UStaticMesh>>& Entry : ObjectMeshMap)
	{
		if (Name.Contains(Entry.Key))
		{
			return Entry.Value.LoadSynchronous();
		}
	}

	return nullptr;
}
