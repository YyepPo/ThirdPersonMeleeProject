// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"

class AInventoryItemBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPickedUpItemSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();
	UFUNCTION(BlueprintCallable)
		void AddItem(AInventoryItemBase* ItemToAdd);
	void RemoveItem(AInventoryItemBase* ItemToRemove);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
		TArray<AInventoryItemBase*> Items;

	UPROPERTY(BlueprintAssignable,Category = "EventDispatchers ")
		FPickedUpItemSignature OnItemPickUp;
protected:
	virtual void BeginPlay() override;
private:
	AInventoryItemBase* StockItem(AInventoryItemBase* ItemToAdd);
};
