// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class ATreasure;
UCLASS()
class SLASH_API ABreakableActor : public AActor,public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();

protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& HitLocation,AActor* Hitter) override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		UGeometryCollectionComponent* GeometryCollection;

private:
	UPROPERTY(EditAnywhere)
		float ScaleInterpSpeed;
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<ATreasure>> TreasureObjects;
	UPROPERTY(EditAnywhere)
		float ZOffSet;
	bool hasBeenHit;
	bool hasBeenSpawn;
};
