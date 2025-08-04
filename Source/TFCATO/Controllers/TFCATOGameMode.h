// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TFCATOGameMode.generated.h"

class UObjectModel;

DECLARE_LOG_CATEGORY_EXTERN(LogTFCATOGameMode, Log, All);

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

	UFUNCTION(BlueprintPure)
	UObjectModel* GetObjectModel() const { return ObjectModel.Get(); }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Settings")
	TMap<FString, TSoftObjectPtr<UStaticMesh>> ObjectMeshMap;

private:
	TWeakObjectPtr<UObjectModel> ObjectModel = nullptr;
};
