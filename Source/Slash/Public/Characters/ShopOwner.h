// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/Interaction.h"
#include "ShopOwner.generated.h"

class UCapsuleComponent;
class ASlashCharacter;
class AInventoryItemBase;

UCLASS()
class SLASH_API AShopOwner : public AActor,public IInteraction
{
	GENERATED_BODY()
	
public:	
	AShopOwner();
	void Interaction() override;
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = "Items")
		TArray<TSubclassOf<AInventoryItemBase>> Items;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere)
		UCapsuleComponent* CapsuleCollider;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY()
		ASlashCharacter* Player;
	UPROPERTY()
		APlayerController* PlayerController;
	UPROPERTY()
		UUserWidget* PlayerInventory;
	UPROPERTY()
		UUserWidget* ShopInventory;
	UPROPERTY(EditDefaultsOnly,Category = "Inventory Class")
		TSubclassOf<class UUserWidget> PlayerInventoryUI;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory Class")
		TSubclassOf<class UUserWidget> ShopInventoryUI;
};
