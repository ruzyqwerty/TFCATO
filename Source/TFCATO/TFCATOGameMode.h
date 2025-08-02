// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TFCATOGameMode.generated.h"

class UObjectModel;

UCLASS(minimalapi)
class ATFCATOGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATFCATOGameMode();

	// Begin AActor override
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End AActor override

	UStaticMesh* GetMeshByName(const FString& Name) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Settings")
	TMap<FString, TSoftObjectPtr<UStaticMesh>> ObjectMeshMap;

private:

	UPROPERTY()
	UObjectModel* ObjectModel = nullptr;
};
