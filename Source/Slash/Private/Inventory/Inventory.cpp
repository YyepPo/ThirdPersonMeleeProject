#include "Inventory/Inventory.h"
#include "Inventory/InventoryItemBase.h"

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventory::BeginPlay()
{
	Super::BeginPlay();	
}

void UInventory::AddItem(AInventoryItemBase* ItemToAdd)
{
	if (StockItem(ItemToAdd))
		return;

	Items.Add(ItemToAdd);
	OnItemPickUp.Broadcast();
}

void UInventory::RemoveItem(AInventoryItemBase* ItemToRemove)
{
	if (ItemToRemove) Items.Remove(ItemToRemove);
	if (GEngine) GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Green, "Item has been removed");
	OnItemPickUp.Broadcast();
}

AInventoryItemBase* UInventory::StockItem(AInventoryItemBase* ItemToAdd)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i]->ItemClass == ItemToAdd->ItemClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("QWEQWEQWEQW"));
			Items[i]->IncrementCurrentStockItem();
			return Items[i];
		}
	}
	return nullptr;
}


