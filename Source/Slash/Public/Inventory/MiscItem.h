#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItemBase.h"
#include "MiscItem.generated.h"

UCLASS()
class SLASH_API AMiscItem : public AInventoryItemBase
{
	GENERATED_BODY()
public:
	void UseItem_Implementation() override;
private:
	UPROPERTY(EditAnywhere)
		float SellAmount;
};
