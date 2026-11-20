// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "Chunk.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateLevel();
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelGenerator::GenerateLevel()
{
	// Validation
	if (!StartChunk)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelGenerator: StartChunk is not set!"));
		return;
	}

	if (NormalChunkClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelGenerator: No NormalChunkClasses provided!"));
		return;
	}

	if (!EndChunkClass)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelGenerator: EndChunkClass is not set!"));
		return;
	}

	// The first connection point is the end of the pre-placed start chunk
	FTransform CurrentConnectionTransform = StartChunk->GetEndTransform();

	// Spawn Normal Chunks
	for (int32 i = 0; i < NumberOfNormalChunks; ++i)
	{
		// Pick a random chunk class from the list
		int32 RandomIndex = FMath::RandRange(0, NormalChunkClasses.Num() - 1);
		TSubclassOf<AChunk> ChunkClass = NormalChunkClasses[RandomIndex];

		AChunk* NewChunk = SpawnAndAlignChunk(ChunkClass, CurrentConnectionTransform);
		if (!NewChunk)
		{
			UE_LOG(LogTemp, Error, TEXT("LevelGenerator: Failed to spawn normal chunk at index %d"), i);
			return;
		}

		// The new connection point is the end of the newly spawned chunk
		CurrentConnectionTransform = NewChunk->GetEndTransform();
	}

	// Spawn the End Chunk
	AChunk* FinalChunk = SpawnAndAlignChunk(EndChunkClass, CurrentConnectionTransform);
	if (!FinalChunk)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelGenerator: Failed to spawn EndChunk."));
	}
}

AChunk* ALevelGenerator::SpawnAndAlignChunk(TSubclassOf<AChunk> ChunkClass, const FTransform& TargetTransform)
{
	if (!ChunkClass) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn the chunk at a temporary location first
	AChunk* NewChunk = GetWorld()->SpawnActor<AChunk>(ChunkClass, FTransform::Identity, SpawnParams);
	if (!NewChunk)
	{
		return nullptr;
	}

	// Get the StartPoint arrow component and its relative transform.
	UArrowComponent* StartArrow = NewChunk->StartPoint;
	if (!StartArrow)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelGenerator: Chunk %s has no StartPoint arrow!"), *NewChunk->GetName());
		NewChunk->Destroy();
		return nullptr;
	}

	const FTransform StartRelativeTransform = StartArrow->GetRelativeTransform();

	// Calculate the desired actor transform so that StartPoint aligns with TargetTransform
	const FTransform DesiredActorTransform = StartRelativeTransform.Inverse() * TargetTransform;

	// Apply the calculated transform
	NewChunk->SetActorTransform(DesiredActorTransform);

	return NewChunk;
}