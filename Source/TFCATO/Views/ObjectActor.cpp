// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectActor.h"
#include "TFCATO/TFCATOGameMode.h"
#include "TFCATO/Models/ObjectModel.h"

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
	UObjectModel* ObjectModel = GameMode->GetObjectModel();
	if (ObjectModel == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't get object model from %s"), *GameMode->GetName());
		return;
	}

	ObjectModel->OnObjectUpdated.AddDynamic(this, &AObjectActor::OnModelUpdated);

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

	UpdateColor();
}

void AObjectActor::OnModelUpdated(const FObjectData& NewData)
{
	if (NewData.Id != ObjectData.Id)
	{
		return;
	}

	ObjectData = NewData;
	UpdateColor();
}

void AObjectActor::UpdateColor() const
{
	FLinearColor ColorValue;
	if (ObjectData.bIsActive)
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
