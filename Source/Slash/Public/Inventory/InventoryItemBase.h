// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/Interaction.h"
#include "InventoryItemBase.generated.h"

class UCapsuleComponent;
class UTexture2D;
class UInventory;
class ASlashCharacter;

UENUM(BlueprintType)
enum class EItemTypes : uint8
{
	ETT_WeaponArmor UMETA(DisplayName = "Weapon&Armor"),
	ETT_Potion UMETA(DisplayName = "Potions"),
	ETT_Misc UMETA(DisplayName = "Misc")
};

UCLASS()
class SLASH_API AInventoryItemBase : public AActor,public IInteraction
{
	GENERATED_BODY()
protected:
	virtual void Interaction() override;
	void UseItem_Implementation() override;
public:	
	AInventoryItemBase();
	void BuyItem_Implementation();
	UFUNCTION(BlueprintCallable)
		void OnRemoveItem();
	UFUNCTION(BlueprintImplementableEvent)
		void OnItemInteractEvent();
	UPROPERTY(EditAnywhere, Category = "Item", BlueprintReadOnly)
		class UTexture2D* Thumbnail;
	//getters
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FORCEINLINE ASlashCharacter* GetPlayer() { return SlashCharacter; }

	UFUNCTION(BlueprintCallable,BlueprintPure)
		FString GetItemName() { return Name; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FString GetItemDescription() { return Description; }
	FORCEINLINE bool GetIsMiscItem() { return ItemType == EItemTypes::ETT_Misc; }

	UFUNCTION(BlueprintCallable,BlueprintPure)
		FORCEINLINE bool GetIsStockable() { return isStockable; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE int32 GetCurrentStockItem() { return CurrentStockItem; }

	UFUNCTION()
	void IncrementCurrentStockItem();
	UFUNCTION(BlueprintCallable)
	void DecrementCurrentStockItem();

	UPROPERTY(EditAnywhere)
		TSubclassOf<AInventoryItemBase> ItemClass;

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
		UInventory* Inventory;
	UPROPERTY()
		ASlashCharacter* SlashCharacter;

	UPROPERTY(VisibleAnywhere)
		UCapsuleComponent* Collider;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
		FString Name = "Item Name";
	UPROPERTY(EditDefaultsOnly, Category = "Item")
		FString Description = "Item/Description";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"),Category = "Item")
		EItemTypes ItemType;
	UPROPERTY(EditDefaultsOnly, Category = "Item")
		USoundBase* UseSound;
	UPROPERTY(EditDefaultsOnly, Category = "Item")
		USoundBase* InteractionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Stock")
		bool isStockable;
	UPROPERTY(VisibleAnywhere, Category = "Stock")
		int32 CurrentStockItem = 1;
	bool canStock;

	UFUNCTION()
		void PlaySound(USoundBase* SoundToPlay);
};
