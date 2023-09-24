// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Treasure/ItemInteractionInterface.h"
#include "Treasure.generated.h"

UCLASS()
class SLASH_API ATreasure : public AItem,public IItemInteractionInterface
{
	GENERATED_BODY()
protected:
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	AActor* OverlapingActor;
	UPROPERTY(EditAnywhere)
		int32 GoldAmount;
public:
	FORCEINLINE int32 GetGoldAmount() { return GoldAmount; }
	virtual void UseItem();
};
