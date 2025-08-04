// Copyright Epic Games, Inc. All Rights Reserved.

#include "TFCATOGameMode.h"
#include "TFCATOCharacter.h"
#include "TFCATO/Models/ObjectData.h"
#include "TFCATO/Models/ObjectModel.h"
#include "TFCATO/Views/ObjectActor.h"

DEFINE_LOG_CATEGORY(LogTFCATOGameMode);

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
		UE_LOG(LogTFCATOCharacter, Error, TEXT("%hs :: Can't create ObjectModel"), __FUNCTION__);
		return;
	}

	const FString DefaultPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data/InitialData.json"));
	if (!ObjectModel->LoadFromJSON(DefaultPath))
	{
		UE_LOG(LogTFCATOGameMode, Error, TEXT("%hs :: Failed to load JSON data from %s"), __FUNCTION__, *DefaultPath);
		return;
	}

	UE_LOG(LogTFCATOGameMode, Log, TEXT("Loaded %d objects from JSON"), ObjectModel->ObjectDataList.Num());
	for (const FObjectData& Data : ObjectModel->ObjectDataList)
	{
		UE_LOG(LogTFCATOGameMode, Log, TEXT("Object: %s | Color: %s | Active: %s"), *Data.Name.ToString(), *Data.Color, Data.bIsActive ? TEXT("true") : TEXT("false"));

		AObjectActor* SpawnedActor = GetWorld()->SpawnActor<AObjectActor>(AObjectActor::StaticClass(), Data.Position, FRotator::ZeroRotator);
		if (SpawnedActor == nullptr)
		{
			UE_LOG(LogTFCATOGameMode, Error, TEXT("%hs :: Failed to spawn actor: %s"), __FUNCTION__, *Data.Name.ToString());
			continue;
		}

		SpawnedActor->InitializeFromData(Data);
	}
}

void ATFCATOGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const FString DefaultPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data/InitialData.json"));
	ObjectModel->SaveToJson(DefaultPath);

	Super::EndPlay(EndPlayReason);
}

UStaticMesh* ATFCATOGameMode::GetMeshByName(const FString& Name) const
{
	for (const TPair<FString, TSoftObjectPtr<UStaticMesh>>& Entry : ObjectMeshMap)
	{
		if (Name.ToLower().Contains(Entry.Key))
		{
			return Entry.Value.LoadSynchronous();
		}
	}

	return nullptr;
}
