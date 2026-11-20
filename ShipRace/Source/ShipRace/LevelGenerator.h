// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

class AChunk;

UCLASS()
class SHIPRACE_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Custom|Generation")
	AChunk* StartChunk;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Generation")
	TArray<TSubclassOf<AChunk>> NormalChunkClasses;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Generation")
	TSubclassOf<AChunk> EndChunkClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Generation", meta = (ClampMin = "0"))
	int32 NumberOfNormalChunks = 2;
	
	UFUNCTION(BlueprintCallable, Category = "Custom|Generation")
	void GenerateLevel();
	
private:
	AChunk* SpawnAndAlignChunk(TSubclassOf<AChunk> ChunkClass, const FTransform& TargetTransform);
};
