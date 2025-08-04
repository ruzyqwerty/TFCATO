// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectActor.h"
#include "TFCATO/Controllers/TFCATOGameMode.h"
#include "TFCATO/Models/ObjectModel.h"

DEFINE_LOG_CATEGORY(LogObjectActor);

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
		UE_LOG(LogObjectActor, Error, TEXT("%hs :: MeshComponent is invalid in %s"), __FUNCTION__, *GetName());
		return;
	}

	const ATFCATOGameMode* GameMode = Cast<ATFCATOGameMode>(GetWorld()->GetAuthGameMode());
	UObjectModel* ObjectModel = GameMode->GetObjectModel();
	if (ObjectModel == nullptr)
	{
		UE_LOG(LogObjectActor, Error, TEXT("%hs :: Can't get object model from %s"), __FUNCTION__, *GameMode->GetName());
		return;
	}

	ObjectModel->OnObjectUpdated.AddDynamic(this, &AObjectActor::OnModelUpdated);

	UStaticMesh* MeshByName = GameMode->GetMeshByName(ObjectData.Name.ToString());
	if (MeshByName == nullptr)
	{
		UE_LOG(LogObjectActor, Error, TEXT("%hs :: No mesh found for %s"), __FUNCTION__, *ObjectData.Name.ToString());
		return;
	}

	MeshComponent->SetStaticMesh(MeshByName);

	DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynamicMaterial.IsValid())
	{
		UE_LOG(LogObjectActor, Error, TEXT("%hs :: Can't create dynamic material for %s in %s"), __FUNCTION__, *MeshComponent->GetName(), *GetName());
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
