// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UCapsuleComponent;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_SinusoidalMovement,
	EIS_Equipped
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon UMETA(Display = "Weapon"),
	EIT_Miscellaneous UMETA(Displauy = "Misc")
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ItemMesh;

	EItemState ItemState = EItemState::EIS_SinusoidalMovement;

	void OnItemInteraction();
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Equip(USceneComponent* Parent, FName SocketName, APawn* NewInsitagor, APawn* NewOwner);
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundWave* CollectSound;
private:
	UPROPERTY(EditDefaultsOnly)
		EItemType ItemType;

	UPROPERTY(EditAnywhere)
		UCapsuleComponent* CapsuleCollider;
	UPROPERTY(EditAnywhere)
		UNiagaraSystem* CollectVFX;
	void AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName);
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUserWidget> InteractionWidget;
	UUserWidget* Widget;
	APlayerController* PlayerController;
public:
	UCapsuleComponent* GetCapsuleComponent() { return CapsuleCollider; }

#pragma region Sin & Cos oidal Movement

private:
	UPROPERTY(VisibleAnywhere)
		float Time;
	UPROPERTY(EditAnywhere)
		float Speed;
	UPROPERTY(EditAnywhere)
		float Amplitude;
public:
	UFUNCTION(BlueprintCallable,BlueprintPure)
		float GetSinusoidalValue();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetCosinusoidalValue();

#pragma endregion 
};
