// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectActor.h"

#include "TFCATO/TFCATOGameMode.h"

AObjectActor::AObjectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
}

void AObjectActor::InitializeFromData(const FObjectData& InData)
{
	ObjectData = InData;

	if (MeshComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MeshComponent is invalid in %s"), *GetName());
		return;
	}

	const ATFCATOGameMode* GameMode = Cast<ATFCATOGameMode>(GetWorld()->GetAuthGameMode());
	UStaticMesh* MeshByName = GameMode->GetMeshByName(ObjectData.Name.ToString());
	if (MeshByName == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No mesh found for %s"), *ObjectData.Name.ToString());
		return;
	}

	MeshComponent->SetStaticMesh(MeshByName);

	DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynamicMaterial.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Can't create dynamic material for %s in %s"), *MeshComponent->GetName(), *GetName());
		return;
	}

	SetIsActive(ObjectData.bIsActive);
}

void AObjectActor::SetIsActive(const bool bIsActive)
{
	ObjectData.bIsActive = bIsActive;

	FLinearColor ColorValue;
	if (bIsActive)
	{
		const FString ColorName = ObjectData.Color.ToLower();
		if (ColorName == "red")
		{
			ColorValue = FLinearColor::Red;
		}
		else if (ColorName == "blue")
		{
			ColorValue = FLinearColor::Blue;
		}
		else if (ColorName == "green")
		{
			ColorValue = FLinearColor::Green;
		}
		else
		{
			ColorValue = FLinearColor::White;
		}
	}
	else
	{
		ColorValue = FLinearColor::Gray;
	}

	DynamicMaterial->SetVectorParameterValue(FName("DiffuseColor"), ColorValue);
}
